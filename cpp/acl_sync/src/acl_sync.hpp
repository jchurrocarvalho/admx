#pragma once

#include <cstdint>
#include <string>

namespace aclsync {

enum class EntityType {
  User,
  Group
};

enum class OwnerClass {
  UserOwner,
  GroupOwner
};

struct Request {
  std::string target_path;
  std::string source_path;
  EntityType entity_type;
  uint32_t entity_id;
  OwnerClass owner_class;
};

[[nodiscard]] bool parse_entity_type(const std::string& value, EntityType& out);
[[nodiscard]] bool parse_owner_class(const std::string& value, OwnerClass& out);
[[nodiscard]] bool parse_u32(const std::string& value, uint32_t& out);
[[nodiscard]] bool resolve_name_or_id(const std::string& value, EntityType entity_type, uint32_t& out);

[[nodiscard]] int apply_request(const Request& request, std::string& error);

} // namespace aclsync
