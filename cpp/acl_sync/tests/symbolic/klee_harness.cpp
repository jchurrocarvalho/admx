#include "../../src/acl_sync.hpp"

#include <cstdint>

extern "C" void klee_assume(unsigned long condition);
extern "C" int klee_range(int begin, int end, const char* name);

int main() {
  int v0 = klee_range(0, 255, "c0");
  int v1 = klee_range(0, 255, "c1");
  int v2 = klee_range(0, 255, "c2");
  int v3 = klee_range(0, 255, "c3");

  char s[5] = {static_cast<char>(v0), static_cast<char>(v1), static_cast<char>(v2), static_cast<char>(v3), '\0'};
  for (int i = 0; i < 4; ++i) {
    klee_assume(s[i] == 'u' || s[i] == 's' || s[i] == 'e' || s[i] == 'r' || s[i] == 'g' || s[i] == 'o' || s[i] == 'p');
  }

  aclsync::EntityType type;
  (void)aclsync::parse_entity_type(s, type);

  uint32_t value = 0;
  (void)aclsync::parse_u32(s, value);

  return 0;
}
