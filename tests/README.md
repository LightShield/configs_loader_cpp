# Tests

## Building and Running Tests

From the project root:

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

## Test Suites

### config_tests.cpp
Tests for the `Config<T>` struct:
- Default values
- Required/optional fields
- Flag storage
- Verifier functions
- Type support (string, int, bool, double)
- Reset functionality

### configs_loader_tests.cpp
Tests for the `ConfigsLoader<T>` class:
- CLI argument parsing (multiple formats)
- Required field validation
- Preset flag reservation
- Initialization state tracking
- Error handling

## Test Framework

Uses Google Test (GTest) v1.14.0, automatically fetched via CMake FetchContent.

## Running Specific Tests

```bash
# Run only Config tests
./config_tests

# Run only ConfigsLoader tests
./configs_loader_tests

# Run specific test
./config_tests --gtest_filter=ConfigTest.DefaultValueIsSet
```

## Test Coverage

29 tests covering all major functionality.
