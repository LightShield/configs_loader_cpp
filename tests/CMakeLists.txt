# Fetch GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

function(add_configs_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE configs_loader_cpp GTest::gtest_main)
endfunction()

add_configs_test(config_tests config_tests.cpp)
add_configs_test(configs_loader_tests configs_loader_tests.cpp)
add_configs_test(help_generator_tests help/help_generator_tests.cpp)
add_configs_test(cli_argument_parser_tests cli/cli_argument_parser_tests.cpp)
add_configs_test(toml_deserializer_tests serialization/toml_deserializer_tests.cpp)

include(GoogleTest)
gtest_discover_tests(config_tests)
gtest_discover_tests(configs_loader_tests)
gtest_discover_tests(cli_argument_parser_tests)
gtest_discover_tests(help_generator_tests)
gtest_discover_tests(toml_deserializer_tests)
