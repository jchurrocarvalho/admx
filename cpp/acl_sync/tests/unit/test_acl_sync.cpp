#include "acl_sync.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#if defined(ACLSYNC_HAS_POSIX_ACL)
#include <sys/acl.h>
#endif

namespace {

int g_failures = 0;

void expect_true(bool cond, const std::string& message) {
  if (!cond) {
    std::cerr << "FAIL: " << message << "\n";
    g_failures += 1;
  }
}

#if defined(ACLSYNC_HAS_POSIX_ACL)
int read_acl_bits_for_uid(const std::string& path, uid_t uid) {
  acl_t acl = acl_get_file(path.c_str(), ACL_TYPE_ACCESS);
  if (acl == nullptr) return -1;

  acl_entry_t entry;
  int entry_id = ACL_FIRST_ENTRY;
  while (acl_get_entry(acl, entry_id, &entry) == 1) {
    entry_id = ACL_NEXT_ENTRY;
    acl_tag_t tag;
    if (acl_get_tag_type(entry, &tag) != 0 || tag != ACL_USER) continue;

    void* qualifier = acl_get_qualifier(entry);
    if (qualifier == nullptr) continue;

    const uid_t entry_uid = *static_cast<uid_t*>(qualifier);
    acl_free(qualifier);
    if (entry_uid != uid) continue;

    acl_permset_t permset;
    if (acl_get_permset(entry, &permset) != 0) {
      acl_free(acl);
      return -1;
    }

    int bits = 0;
    if (acl_get_perm(permset, ACL_READ) == 1) bits |= 0b100;
    if (acl_get_perm(permset, ACL_WRITE) == 1) bits |= 0b010;
    if (acl_get_perm(permset, ACL_EXECUTE) == 1) bits |= 0b001;

    acl_free(acl);
    return bits;
  }
  acl_free(acl);
  return -1;
}
#endif

#if defined(ACLSYNC_HAS_POSIX_ACL)
std::string make_tmp_dir() {
  std::string tpl = "/tmp/acl_sync_testXXXXXX";
  std::unique_ptr<char[]> cstr(new char[tpl.size() + 1]);
  std::snprintf(cstr.get(), tpl.size() + 1, "%s", tpl.c_str());
  char* tmp = mkdtemp(cstr.get());
  if (tmp == nullptr) return {};
  return tmp;
}
#endif

} // namespace

int main() {
  aclsync::EntityType et;
  expect_true(aclsync::parse_entity_type("user", et) && et == aclsync::EntityType::User, "parse user entity type");
  expect_true(aclsync::parse_entity_type("group", et) && et == aclsync::EntityType::Group, "parse group entity type");

  aclsync::OwnerClass oc;
  expect_true(aclsync::parse_owner_class("owner-user", oc) && oc == aclsync::OwnerClass::UserOwner, "parse owner-user");
  expect_true(aclsync::parse_owner_class("owner-group", oc) && oc == aclsync::OwnerClass::GroupOwner, "parse owner-group");

  uint32_t value = 0;
  expect_true(aclsync::parse_u32("123", value) && value == 123, "parse_u32 accepts numeric");
  expect_true(!aclsync::parse_u32("-1", value), "parse_u32 rejects negative");

#if defined(ACLSYNC_HAS_POSIX_ACL)
  const std::string tmp_dir = make_tmp_dir();
  expect_true(!tmp_dir.empty(), "created temp directory");

  const std::string source = tmp_dir + "/source.txt";
  const std::string target = tmp_dir + "/target.txt";
  {
    std::ofstream(source) << "source";
    std::ofstream(target) << "target";
  }

  chmod(source.c_str(), 0740);
  chmod(target.c_str(), 0600);

  aclsync::Request from_other{target, source, aclsync::EntityType::User, static_cast<uint32_t>(getuid()), aclsync::OwnerClass::UserOwner};
  std::string error;
  expect_true(aclsync::apply_request(from_other, error) == 0, "apply_request from another file");
  expect_true(read_acl_bits_for_uid(target, getuid()) == 0b111, "copied owner-user bits rwx from source");

  chmod(target.c_str(), 0620);
  aclsync::Request same_file{target, "", aclsync::EntityType::User, static_cast<uint32_t>(getuid()), aclsync::OwnerClass::GroupOwner};
  expect_true(aclsync::apply_request(same_file, error) == 0, "apply_request in same file");
  expect_true(read_acl_bits_for_uid(target, getuid()) == 0b010, "copied owner-group bits from same file");

  std::system(("rm -rf " + tmp_dir).c_str());
#else
  std::string err;
  const aclsync::Request req{"/tmp/none", "", aclsync::EntityType::User, 1U, aclsync::OwnerClass::UserOwner};
  expect_true(aclsync::apply_request(req, err) != 0, "stub apply_request fails safely without ACL support");
#endif

  if (g_failures != 0) {
    std::cerr << g_failures << " test(s) failed\n";
    return 1;
  }
  std::cout << "All tests passed\n";
  return 0;
}
