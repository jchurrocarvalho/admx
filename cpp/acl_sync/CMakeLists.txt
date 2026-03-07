cmake_minimum_required(VERSION 3.20)

project(acl_sync VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SYMBOLIC_TESTS "Enable symbolic execution tests (requires KLEE)" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wformat=2
    -Werror=format-security
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=3
    -fno-omit-frame-pointer
  )
  add_link_options(-Wl,-z,relro,-z,now)
endif()

find_path(ACL_INCLUDE_DIR sys/acl.h)
find_library(ACL_LIBRARY NAMES acl)

add_library(acl_sync_lib src/acl_sync.cpp)
target_include_directories(acl_sync_lib PUBLIC src)

if(ACL_INCLUDE_DIR AND ACL_LIBRARY)
  target_include_directories(acl_sync_lib PRIVATE ${ACL_INCLUDE_DIR})
  target_link_libraries(acl_sync_lib PUBLIC ${ACL_LIBRARY})
  target_compile_definitions(acl_sync_lib PUBLIC ACLSYNC_HAS_POSIX_ACL=1)
else()
  message(WARNING "POSIX ACL headers/library not found. Build will succeed with runtime stub; install libacl-devel on RHEL 9+.")
endif()

add_executable(acl-sync src/main.cpp)
target_link_libraries(acl-sync PRIVATE acl_sync_lib)

include(CTest)
if(BUILD_TESTING)
  add_executable(unit_acl_sync tests/unit/test_acl_sync.cpp)
  target_link_libraries(unit_acl_sync PRIVATE acl_sync_lib)
  add_test(NAME unit_acl_sync COMMAND unit_acl_sync)

  if(ENABLE_SYMBOLIC_TESTS)
    find_program(KLEE_BINARY klee)
    if(KLEE_BINARY)
      add_test(NAME symbolic_acl_sync COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/symbolic/run_klee.sh ${CMAKE_CURRENT_SOURCE_DIR})
    else()
      message(WARNING "KLEE not found; symbolic tests disabled")
    endif()
  endif()
endif()
