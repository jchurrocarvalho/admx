#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="${1:?root dir required}"
BUILD_DIR="${ROOT_DIR}/build/symbolic"
mkdir -p "${BUILD_DIR}"

cd "${ROOT_DIR}"

clang++ -I./src -std=c++20 -emit-llvm -c -g -O0 \
  ./src/acl_sync.cpp ./tests/symbolic/klee_harness.cpp

llvm-link acl_sync.bc klee_harness.bc -o "${BUILD_DIR}/acl_sync_klee.bc"
rm -f acl_sync.bc klee_harness.bc

klee --exit-on-error "${BUILD_DIR}/acl_sync_klee.bc" >/dev/null

echo "KLEE run completed"
