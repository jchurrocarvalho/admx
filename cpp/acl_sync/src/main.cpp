#include "acl_sync.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

void print_usage() {
  std::cerr
      << "Usage:\n"
      << "  acl-sync --target <file|dir> [--source <file|dir>] --entity-type <user|group>\\n"
      << "           --entity <name|id> --copy <owner-user|owner-group>\n\n"
      << "Examples:\n"
      << "  acl-sync --target /tmp/a --entity-type user --entity alice --copy owner-user\n"
      << "  acl-sync --target /tmp/a --source /tmp/b --entity-type group --entity devops --copy owner-group\n";
}

bool read_arg(const std::vector<std::string>& args, std::size_t& i, std::string& out) {
  if (i + 1 >= args.size()) {
    return false;
  }
  out = args[i + 1];
  i += 1;
  return true;
}

} // namespace

int main(int argc, char* argv[]) {
  std::vector<std::string> args;
  args.reserve(static_cast<std::size_t>(argc));
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  aclsync::Request request;
  bool has_target = false;
  bool has_entity = false;
  bool has_entity_type = false;
  bool has_copy = false;

  for (std::size_t i = 0; i < args.size(); ++i) {
    if (args[i] == "--target") {
      has_target = read_arg(args, i, request.target_path);
    } else if (args[i] == "--source") {
      if (!read_arg(args, i, request.source_path)) {
        std::cerr << "Missing value for --source\n";
        return 2;
      }
    } else if (args[i] == "--entity-type") {
      std::string value;
      if (!read_arg(args, i, value) || !aclsync::parse_entity_type(value, request.entity_type)) {
        std::cerr << "Invalid value for --entity-type\n";
        return 2;
      }
      has_entity_type = true;
    } else if (args[i] == "--entity") {
      std::string value;
      if (!read_arg(args, i, value)) {
        std::cerr << "Missing value for --entity\n";
        return 2;
      }
      if (!has_entity_type) {
        std::cerr << "--entity-type must be provided before --entity\n";
        return 2;
      }
      if (!aclsync::resolve_name_or_id(value, request.entity_type, request.entity_id)) {
        std::cerr << "Invalid value for --entity (name or numeric id expected)\n";
        return 2;
      }
      has_entity = true;
    } else if (args[i] == "--copy") {
      std::string value;
      if (!read_arg(args, i, value) || !aclsync::parse_owner_class(value, request.owner_class)) {
        std::cerr << "Invalid value for --copy\n";
        return 2;
      }
      has_copy = true;
    } else if (args[i] == "--help" || args[i] == "-h") {
      print_usage();
      return 0;
    } else {
      std::cerr << "Unknown argument: " << args[i] << "\n";
      print_usage();
      return 2;
    }
  }

  if (!has_target || !has_entity_type || !has_entity || !has_copy) {
    print_usage();
    return 2;
  }

  std::string error;
  const int rc = aclsync::apply_request(request, error);
  if (rc != 0) {
    std::cerr << "Error: " << error << "\n";
    return rc;
  }

  std::cout << "ACL updated successfully\n";
  return 0;
}
