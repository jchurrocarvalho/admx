#include "acl_sync.hpp"

#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#if defined(ACLSYNC_HAS_POSIX_ACL)
#include <acl/libacl.h>
#include <sys/acl.h>
#endif

namespace aclsync {
namespace {

#if defined(ACLSYNC_HAS_POSIX_ACL)
struct QualifierDeleter {
  void operator()(void* q) const {
    if (q != nullptr) {
      acl_free(q);
    }
  }
};
#endif

[[nodiscard]] bool is_regular_or_dir(const struct stat& st) {
  return S_ISREG(st.st_mode) || S_ISDIR(st.st_mode);
}

#if defined(ACLSYNC_HAS_POSIX_ACL)
[[nodiscard]] int owner_mode_to_acl_bits(mode_t mode, OwnerClass owner_class) {
  int bits = 0;
  if (owner_class == OwnerClass::UserOwner) {
    if ((mode & S_IRUSR) != 0) bits |= 0b100;
    if ((mode & S_IWUSR) != 0) bits |= 0b010;
    if ((mode & S_IXUSR) != 0) bits |= 0b001;
  } else {
    if ((mode & S_IRGRP) != 0) bits |= 0b100;
    if ((mode & S_IWGRP) != 0) bits |= 0b010;
    if ((mode & S_IXGRP) != 0) bits |= 0b001;
  }
  return bits;
}

[[nodiscard]] bool qualifier_matches(acl_entry_t entry, EntityType entity_type, uint32_t entity_id) {
  acl_tag_t tag_type;
  if (acl_get_tag_type(entry, &tag_type) != 0) {
    return false;
  }
  if (entity_type == EntityType::User && tag_type != ACL_USER) return false;
  if (entity_type == EntityType::Group && tag_type != ACL_GROUP) return false;

  std::unique_ptr<void, QualifierDeleter> qualifier(acl_get_qualifier(entry));
  if (!qualifier) return false;

  if (entity_type == EntityType::User) {
    return static_cast<uint32_t>(*static_cast<uid_t*>(qualifier.get())) == entity_id;
  }
  return static_cast<uint32_t>(*static_cast<gid_t*>(qualifier.get())) == entity_id;
}

[[nodiscard]] bool set_permset(acl_entry_t entry, int bits, std::string& error) {
  acl_permset_t permset;
  if (acl_get_permset(entry, &permset) != 0) {
    error = "Failed to get ACL permset";
    return false;
  }
  if (acl_clear_perms(permset) != 0) {
    error = "Failed to clear ACL perms";
    return false;
  }
  if ((bits & 0b100) != 0 && acl_add_perm(permset, ACL_READ) != 0) {
    error = "Failed to add read permission";
    return false;
  }
  if ((bits & 0b010) != 0 && acl_add_perm(permset, ACL_WRITE) != 0) {
    error = "Failed to add write permission";
    return false;
  }
  if ((bits & 0b001) != 0 && acl_add_perm(permset, ACL_EXECUTE) != 0) {
    error = "Failed to add execute permission";
    return false;
  }
  return true;
}
#endif

} // namespace

bool parse_entity_type(const std::string& value, EntityType& out) {
  if (value == "user") { out = EntityType::User; return true; }
  if (value == "group") { out = EntityType::Group; return true; }
  return false;
}

bool parse_owner_class(const std::string& value, OwnerClass& out) {
  if (value == "owner-user") { out = OwnerClass::UserOwner; return true; }
  if (value == "owner-group") { out = OwnerClass::GroupOwner; return true; }
  return false;
}

bool parse_u32(const std::string& value, uint32_t& out) {
  if (value.empty()) return false;
  errno = 0;
  char* end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (errno != 0 || end == nullptr || *end != '\0' || parsed > UINT32_MAX) return false;
  out = static_cast<uint32_t>(parsed);
  return true;
}

bool resolve_name_or_id(const std::string& value, EntityType entity_type, uint32_t& out) {
  if (parse_u32(value, out)) return true;
  if (entity_type == EntityType::User) {
    const passwd* user = getpwnam(value.c_str());
    if (user == nullptr) return false;
    out = static_cast<uint32_t>(user->pw_uid);
    return true;
  }
  const group* grp = getgrnam(value.c_str());
  if (grp == nullptr) return false;
  out = static_cast<uint32_t>(grp->gr_gid);
  return true;
}

int apply_request(const Request& request, std::string& error) {
  struct stat target_stat {};
  if (lstat(request.target_path.c_str(), &target_stat) != 0) {
    error = "Cannot stat target path: " + std::string(std::strerror(errno));
    return 1;
  }
  if (S_ISLNK(target_stat.st_mode)) {
    error = "Target path must not be a symbolic link";
    return 1;
  }
  if (!is_regular_or_dir(target_stat)) {
    error = "Target path must be a regular file or directory";
    return 1;
  }

  const std::string source = request.source_path.empty() ? request.target_path : request.source_path;
  struct stat source_stat {};
  if (lstat(source.c_str(), &source_stat) != 0) {
    error = "Cannot stat source path: " + std::string(std::strerror(errno));
    return 1;
  }
  if (S_ISLNK(source_stat.st_mode)) {
    error = "Source path must not be a symbolic link";
    return 1;
  }
  if (!is_regular_or_dir(source_stat)) {
    error = "Source path must be a regular file or directory";
    return 1;
  }

#if !defined(ACLSYNC_HAS_POSIX_ACL)
  (void)request;
  (void)source_stat;
  error = "This binary was built without POSIX ACL support. Install libacl-devel and rebuild.";
  return 1;
#else
  const int bits = owner_mode_to_acl_bits(source_stat.st_mode, request.owner_class);
  acl_t acl = acl_get_file(request.target_path.c_str(), ACL_TYPE_ACCESS);
  if (acl == nullptr) {
    error = "Failed to get target ACL";
    return 1;
  }

  acl_entry_t entry;
  int entry_id = ACL_FIRST_ENTRY;
  bool found = false;
  while (acl_get_entry(acl, entry_id, &entry) == 1) {
    if (qualifier_matches(entry, request.entity_type, request.entity_id)) {
      found = true;
      break;
    }
    entry_id = ACL_NEXT_ENTRY;
  }

  if (!found) {
    if (acl_create_entry(&acl, &entry) != 0) {
      error = "Failed to create ACL entry";
      acl_free(acl);
      return 1;
    }
    const acl_tag_t tag = request.entity_type == EntityType::User ? ACL_USER : ACL_GROUP;
    if (acl_set_tag_type(entry, tag) != 0) {
      error = "Failed to set ACL entry tag";
      acl_free(acl);
      return 1;
    }
    if (request.entity_type == EntityType::User) {
      uid_t uid = static_cast<uid_t>(request.entity_id);
      if (acl_set_qualifier(entry, &uid) != 0) {
        error = "Failed to set ACL user qualifier";
        acl_free(acl);
        return 1;
      }
    } else {
      gid_t gid = static_cast<gid_t>(request.entity_id);
      if (acl_set_qualifier(entry, &gid) != 0) {
        error = "Failed to set ACL group qualifier";
        acl_free(acl);
        return 1;
      }
    }
  }

  if (!set_permset(entry, bits, error)) {
    acl_free(acl);
    return 1;
  }
  if (acl_calc_mask(&acl) != 0) {
    error = "Failed to recalculate ACL mask";
    acl_free(acl);
    return 1;
  }
  if (acl_set_file(request.target_path.c_str(), ACL_TYPE_ACCESS, acl) != 0) {
    error = "Failed to set ACL on target path: " + std::string(std::strerror(errno));
    acl_free(acl);
    return 1;
  }
  acl_free(acl);
  return 0;
#endif
}

} // namespace aclsync
