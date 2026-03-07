# acl-sync (C++ / Clang / CMake)

`acl-sync` is a secure-by-default command-line tool for Red Hat Enterprise Linux 9+ that updates POSIX ACL entries by **copying owner permissions** from UNIX mode bits.

## What it does

It updates (or creates) an ACL entry for a target user/group by copying permissions from:

1. **Another file or directory** (`--source` + `--target`), or
2. **The same file/directory** (omit `--source`; source defaults to target).

You can copy from:
- `owner-user` (mode bits `u+rwx`) or
- `owner-group` (mode bits `g+rwx`).

## Security-focused practices used

- Clang hardening flags (`-fstack-protector-strong`, `-D_FORTIFY_SOURCE=3`, RELRO/NOW).
- Strict warnings (`-Wall -Wextra -Wpedantic -Wconversion ...`).
- Input validation for all CLI arguments.
- Safe numeric parsing and explicit error handling.
- Rejects symlink source/target paths (prevents symlink confusion attacks).
- Restricts operation to regular files or directories.

## Requirements (RHEL 9+)

Install build dependencies:

```bash
sudo dnf install -y clang cmake make libacl-devel llvm
```

For symbolic execution tests (optional):

```bash
sudo dnf install -y klee
```

## Build

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -DBUILD_TESTING=ON
cmake --build build
```

## Run

### 1) Copy from another file/directory

Copy owner-user bits from `/data/source.txt` into an ACL user entry for `alice` on `/data/target.txt`:

```bash
./build/acl-sync \
  --target /data/target.txt \
  --source /data/source.txt \
  --entity-type user \
  --entity alice \
  --copy owner-user
```

### 2) Copy from the same file/directory

Create/update ACL group entry `devops` on `/srv/shared` using that same directory's owner-group mode bits:

```bash
./build/acl-sync \
  --target /srv/shared \
  --entity-type group \
  --entity devops \
  --copy owner-group
```

## Test

Unit tests:

```bash
ctest --test-dir build --output-on-failure
```

Enable symbolic execution test with KLEE:

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -DBUILD_TESTING=ON -DENABLE_SYMBOLIC_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## VS Code instructions

1. Install VS Code extensions:
   - **C/C++** (Microsoft)
   - **CMake Tools** (Microsoft)
2. Open this folder (`cpp/acl_sync`) in VS Code.
3. Press `Ctrl+Shift+P` → `CMake: Select a Kit` → choose **Clang**.
4. Press `Ctrl+Shift+P` → `CMake: Configure`.
5. Press `Ctrl+Shift+P` → `CMake: Build`.
6. Run tests with `CMake: Run Tests`.
7. Optional: add launch config for running `acl-sync` with arguments.

### Suggested `.vscode/settings.json`

```json
{
  "cmake.configureSettings": {
    "CMAKE_CXX_COMPILER": "clang++",
    "BUILD_TESTING": "ON"
  },
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "C_Cpp.default.cppStandard": "c++20"
}
```
