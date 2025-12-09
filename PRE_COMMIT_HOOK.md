# Pre-Commit Hook

This repository includes a git pre-commit hook that automatically runs code formatting and tests before allowing commits.

## What It Does

The pre-commit hook (`/.git/hooks/pre-commit`) performs the following checks:

1. **Formats C++ files** using `clang-format` (if available)
2. **Formats Markdown files** using `prettier` (if available)
3. **Runs the test suite** (if build system is set up)
4. **Runs static analysis** using `cppcheck` (if available, optional)

If any of these checks fail, the commit is aborted.

## Requirements

### Required

- **Git** (obviously)

### Optional (but recommended)

- **clang-format**: For C++ code formatting

  - macOS: `brew install clang-format`
  - Linux: `apt-get install clang-format` or `yum install clang-format`
  - Windows: Download from LLVM releases

- **prettier**: For Markdown formatting

  - `npm install -g prettier`

- **cppcheck**: For static analysis (optional)
  - macOS: `brew install cppcheck`
  - Linux: `apt-get install cppcheck`

## How It Works

1. When you run `git commit`, the hook automatically runs
2. It checks for staged C++ files (`.cpp`, `.hpp`, `.h`, `.cc`, `.cxx`)
3. If found, it formats them with `clang-format` and re-stages them
4. It checks for a build system (CMake, Makefile, or `build/` directory)
5. If found, it attempts to build and run tests
6. If tests fail, the commit is aborted

## Bypassing the Hook

If you need to bypass the hook (not recommended), use:

```bash
git commit --no-verify
```

**Warning**: Only bypass the hook for emergency commits. All code should pass formatting and tests before being committed.

## Configuration

### clang-format

The hook uses `.clang-format` in the repository root. If it doesn't exist, the hook creates a default one.

To customize formatting, edit `.clang-format`. See [clang-format documentation](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).

### Test Execution

The hook looks for tests in this order:

1. `build/bin/vm_tests` or `build/tests/vm_tests`
2. `build/vm_tests`
3. CMake's `ctest` (if `build/` directory exists)

## Troubleshooting

### "clang-format not found"

- Install clang-format (see Requirements above)
- Or the hook will skip formatting (with a warning)

### "Tests failed"

- Fix the failing tests before committing
- The hook shows which tests failed
- Run tests manually: `cd build && ctest --output-on-failure`

### "No test executable found"

- This is normal if the project hasn't been built yet
- Once you set up the build system, tests will be required
- The hook will skip tests until build infrastructure exists

### Hook not running

- Make sure the hook is executable: `chmod +x .git/hooks/pre-commit`
- Verify it exists: `ls -la .git/hooks/pre-commit`

## Manual Testing

You can test the hook manually:

```bash
# Test formatting
git add src/some_file.cpp
git commit -m "test"  # Hook will run

# Test with no changes (should pass)
git commit --allow-empty -m "empty commit"
```

## Disabling the Hook

To temporarily disable the hook:

```bash
# Rename it
mv .git/hooks/pre-commit .git/hooks/pre-commit.disabled

# Re-enable it
mv .git/hooks/pre-commit.disabled .git/hooks/pre-commit
```

## Contributing

When contributing to this project:

1. Make sure your code is formatted (the hook will do this automatically)
2. Make sure all tests pass (the hook will check this)
3. Don't bypass the hook unless absolutely necessary
