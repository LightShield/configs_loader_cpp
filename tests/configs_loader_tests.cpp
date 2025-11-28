#include "configs_loader.hpp"
#include <gtest/gtest.h>

struct TestConfigs {
    Config<std::string> filename{.default_value = "default.txt", .flags = {"--file", "-f"}};
    Config<int> count{.default_value = 10, .flags = {"--count", "-c"}};
    Config<bool> verbose{.default_value = false, .flags = {"--verbose", "-v"}};

    REGISTER_CONFIG_FIELDS(filename, count, verbose)
};

TEST(ConfigsLoaderTest, DefaultConstructorUsesDefaults) {
    ConfigsLoader<TestConfigs> loader;
    EXPECT_EQ(loader.configs.filename.value, "default.txt");
    EXPECT_EQ(loader.configs.count.value, 10);
    EXPECT_FALSE(loader.configs.verbose.value);
}

TEST(ConfigsLoaderTest, ParsesLongFlagWithSpace) {
    const char* argv[] = {"prog", "--file", "test.txt"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
}

TEST(ConfigsLoaderTest, ParsesShortFlagWithSpace) {
    const char* argv[] = {"prog", "-f", "short.txt"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "short.txt");
}

TEST(ConfigsLoaderTest, ParsesLongFlagWithEquals) {
    const char* argv[] = {"prog", "--file=equals.txt"};
    ConfigsLoader<TestConfigs> loader(2, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "equals.txt");
}

TEST(ConfigsLoaderTest, ParsesIntegerValue) {
    const char* argv[] = {"prog", "--count", "42"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.count.value, 42);
}

TEST(ConfigsLoaderTest, ParsesBooleanTrue) {
    const char* argv[] = {"prog", "--verbose", "true"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST(ConfigsLoaderTest, ParsesBooleanOne) {
    const char* argv[] = {"prog", "-v", "1"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST(ConfigsLoaderTest, ParsesMultipleArguments) {
    const char* argv[] = {"prog", "--file", "multi.txt", "--count", "99", "-v", "true"};
    ConfigsLoader<TestConfigs> loader(7, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "multi.txt");
    EXPECT_EQ(loader.configs.count.value, 99);
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST(ConfigsLoaderTest, LastValueWinsForDuplicates) {
    const char* argv[] = {"prog", "--count", "10", "--count", "20"};
    ConfigsLoader<TestConfigs> loader(5, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.count.value, 20);
}

TEST(ConfigsLoaderTest, UnknownFlagsAreIgnored) {
    const char* argv[] = {"prog", "--unknown", "value", "--file", "known.txt"};
    ConfigsLoader<TestConfigs> loader(5, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "known.txt");
}

TEST(ConfigsLoaderTest, RequiredFieldThrowsWhenNotSet) {
    struct RequiredConfigs {
        Config<std::string> required_field{
            .default_value = "",
            .flags = {"--required"},
            .required = true
        };
        REGISTER_CONFIG_FIELDS(required_field)
    };
    
    const char* argv[] = {"prog"};
    EXPECT_THROW({
        ConfigsLoader<RequiredConfigs> loader(1, const_cast<char**>(argv));
    }, std::runtime_error);
}

TEST(ConfigsLoaderTest, RequiredFieldDoesNotThrowWhenSet) {
    struct RequiredConfigs {
        Config<std::string> required_field{
            .default_value = "",
            .flags = {"--required"},
            .required = true
        };
        REGISTER_CONFIG_FIELDS(required_field)
    };
    
    const char* argv[] = {"prog", "--required", "value"};
    EXPECT_NO_THROW({
        ConfigsLoader<RequiredConfigs> loader(3, const_cast<char**>(argv));
    });
}

TEST(ConfigsLoaderTest, MixedFlagFormats) {
    const char* argv[] = {"prog", "--file=mixed.txt", "-c", "50"};
    ConfigsLoader<TestConfigs> loader(4, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "mixed.txt");
    EXPECT_EQ(loader.configs.count.value, 50);
}

TEST(ConfigsLoaderTest, PresetFlagIsReserved) {
    struct BadConfigs {
        Config<std::string> preset{
            .default_value = "",
            .flags = {"--preset"}
        };
        REGISTER_CONFIG_FIELDS(preset)
    };
    
    const char* argv[] = {"prog"};
    EXPECT_THROW({
        ConfigsLoader<BadConfigs> loader(1, const_cast<char**>(argv));
    }, std::runtime_error);
}

TEST(ConfigsLoaderTest, PresetShortFlagIsReserved) {
    struct BadConfigs {
        Config<std::string> my_preset{
            .default_value = "",
            .flags = {"-p"}
        };
        REGISTER_CONFIG_FIELDS(my_preset)
    };
    
    const char* argv[] = {"prog"};
    EXPECT_THROW({
        ConfigsLoader<BadConfigs> loader(1, const_cast<char**>(argv));
    }, std::runtime_error);
}

TEST(ConfigsLoaderTest, IsNotInitializedByDefault) {
    ConfigsLoader<TestConfigs> loader;
    EXPECT_FALSE(loader.is_initialized());
}

TEST(ConfigsLoaderTest, IsInitializedAfterInit) {
    const char* argv[] = {"prog", "--file", "test.txt"};
    ConfigsLoader<TestConfigs> loader;
    loader.Init(3, const_cast<char**>(argv));
    EXPECT_TRUE(loader.is_initialized());
}
