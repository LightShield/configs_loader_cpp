#include "configs_loader.hpp"
#include <gtest/gtest.h>

class ConfigsLoaderTest : public ::testing::Test {
protected:
    struct TestConfigs {
        Config<std::string> filename{
            .default_value = "default.txt", 
            .flags = {"--file", "-f"},
            .description = "Input file to process"
        };
        Config<int> count{
            .default_value = 10, 
            .flags = {"--count", "-c"},
            .description = "Number of iterations"
        };
        Config<bool> verbose{
            .default_value = false, 
            .flags = {"--verbose", "-v"},
            .description = "Enable verbose output"
        };

        REGISTER_CONFIG_FIELDS(filename, count, verbose)
    };
};

TEST_F(ConfigsLoaderTest, DefaultConstructorUsesDefaults) {
    ConfigsLoader<TestConfigs> loader;
    EXPECT_EQ(loader.configs.filename.value, "default.txt");
    EXPECT_EQ(loader.configs.count.value, 10);
    EXPECT_FALSE(loader.configs.verbose.value);
}

TEST_F(ConfigsLoaderTest, ParsesLongFlagWithSpace) {
    const char* argv[] = {"prog", "--file", "test.txt"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
}

TEST_F(ConfigsLoaderTest, ParsesShortFlagWithSpace) {
    const char* argv[] = {"prog", "-f", "test.txt"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
}

TEST_F(ConfigsLoaderTest, ParsesLongFlagWithEquals) {
    const char* argv[] = {"prog", "--file=test.txt"};
    ConfigsLoader<TestConfigs> loader(2, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
}

TEST_F(ConfigsLoaderTest, ParsesIntegerValue) {
    const char* argv[] = {"prog", "--count", "42"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.count.value, 42);
}

TEST_F(ConfigsLoaderTest, ParsesBooleanTrue) {
    const char* argv[] = {"prog", "--verbose", "true"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST_F(ConfigsLoaderTest, ParsesBooleanOne) {
    const char* argv[] = {"prog", "--verbose", "1"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST_F(ConfigsLoaderTest, ParsesMultipleArguments) {
    const char* argv[] = {"prog", "--file", "test.txt", "--count", "5", "--verbose", "true"};
    ConfigsLoader<TestConfigs> loader(7, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
    EXPECT_EQ(loader.configs.count.value, 5);
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST_F(ConfigsLoaderTest, LastValueWinsForDuplicates) {
    const char* argv[] = {"prog", "--count", "5", "--count", "10"};
    ConfigsLoader<TestConfigs> loader(5, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.count.value, 10);
}

TEST_F(ConfigsLoaderTest, UnknownFlagsAreIgnored) {
    const char* argv[] = {"prog", "--unknown", "value", "--file", "test.txt"};
    ConfigsLoader<TestConfigs> loader(5, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
}

TEST_F(ConfigsLoaderTest, RequiredFieldThrowsWhenNotSet) {
    struct RequiredConfigs {
        Config<std::string> required_field{
            .default_value = "",
            .flags = {"--required"},
            .required = true
        };
        REGISTER_CONFIG_FIELDS(required_field)
    };
    
    ConfigsLoader<RequiredConfigs> loader;
    const char* argv[] = {"prog"};
    EXPECT_EQ(loader.init(1, const_cast<char**>(argv)), 1);
    EXPECT_FALSE(loader.is_initialized());
}

TEST_F(ConfigsLoaderTest, RequiredFieldDoesNotThrowWhenSet) {
    struct RequiredConfigs {
        Config<std::string> required_field{
            .default_value = "",
            .flags = {"--required"},
            .required = true
        };
        REGISTER_CONFIG_FIELDS(required_field)
    };
    
    ConfigsLoader<RequiredConfigs> loader;
    const char* argv[] = {"prog", "--required", "value"};
    EXPECT_EQ(loader.init(3, const_cast<char**>(argv)), 0);
    EXPECT_TRUE(loader.is_initialized());
    EXPECT_EQ(loader.configs.required_field.value, "value");
}

TEST_F(ConfigsLoaderTest, MixedFlagFormats) {
    const char* argv[] = {"prog", "-f", "test.txt", "--count=5", "--verbose", "true"};
    ConfigsLoader<TestConfigs> loader(6, const_cast<char**>(argv));
    EXPECT_EQ(loader.configs.filename.value, "test.txt");
    EXPECT_EQ(loader.configs.count.value, 5);
    EXPECT_TRUE(loader.configs.verbose.value);
}

TEST_F(ConfigsLoaderTest, PresetFlagIsReserved) {
    struct BadConfigs {
        Config<std::string> field{
            .default_value = "",
            .flags = {"--preset"}
        };
        REGISTER_CONFIG_FIELDS(field)
    };
    
    ConfigsLoader<BadConfigs> loader;
    const char* argv[] = {"prog"};
    EXPECT_EQ(loader.init(1, const_cast<char**>(argv)), 1);
    EXPECT_FALSE(loader.is_initialized());
}

TEST_F(ConfigsLoaderTest, PresetShortFlagIsReserved) {
    struct BadConfigs {
        Config<std::string> field{
            .default_value = "",
            .flags = {"-p"}
        };
        REGISTER_CONFIG_FIELDS(field)
    };
    
    ConfigsLoader<BadConfigs> loader;
    const char* argv[] = {"prog"};
    EXPECT_EQ(loader.init(1, const_cast<char**>(argv)), 1);
    EXPECT_FALSE(loader.is_initialized());
}

TEST_F(ConfigsLoaderTest, IsNotInitializedByDefault) {
    ConfigsLoader<TestConfigs> loader;
    EXPECT_FALSE(loader.is_initialized());
}

TEST_F(ConfigsLoaderTest, IsInitializedAfterInit) {
    ConfigsLoader<TestConfigs> loader;
    const char* argv[] = {"prog"};
    EXPECT_EQ(loader.init(1, const_cast<char**>(argv)), 0);
    EXPECT_TRUE(loader.is_initialized());
}

TEST_F(ConfigsLoaderTest, MultipleRequiredFieldsReportedTogether) {
    struct MultiRequiredConfigs {
        Config<std::string> field1{
            .default_value = "",
            .flags = {"--field1"},
            .required = true,
            .description = "First required field"
        };
        Config<std::string> field2{
            .default_value = "",
            .flags = {"--field2"},
            .required = true,
            .description = "Second required field"
        };
        Config<std::string> field3{
            .default_value = "",
            .flags = {"--field3"},
            .required = true,
            .description = "Third required field"
        };
        REGISTER_CONFIG_FIELDS(field1, field2, field3)
    };
    
    ConfigsLoader<MultiRequiredConfigs> loader;
    const char* argv[] = {"prog"};
    
    testing::internal::CaptureStderr();
    const int result = loader.init(1, const_cast<char**>(argv));
    const std::string error_output = testing::internal::GetCapturedStderr();
    
    EXPECT_EQ(result, 1);
    EXPECT_FALSE(loader.is_initialized());
    EXPECT_NE(error_output.find("3 error(s)"), std::string::npos);
    EXPECT_NE(error_output.find("--field1"), std::string::npos);
    EXPECT_NE(error_output.find("--field2"), std::string::npos);
    EXPECT_NE(error_output.find("--field3"), std::string::npos);
}

TEST_F(ConfigsLoaderTest, ConfigGroupImplicitConversionWorks) {
    struct NestedConfig {
        Config<int> value{
            .default_value = 42,
            .flags = {"--value"}
        };
        REGISTER_CONFIG_FIELDS(value)
    };
    
    struct GroupedConfigs {
        ConfigGroup<NestedConfig> group{
            .config = {},
            .name_ = "group"
        };
        REGISTER_CONFIG_FIELDS(group)
    };
    
    ConfigsLoader<GroupedConfigs> loader;
    
    // Direct access with .config
    EXPECT_EQ(loader.configs.group.config.value.value, 42);
    
    // Implicit conversion - no .config needed
    const NestedConfig& nested = loader.configs.group;
    EXPECT_EQ(nested.value.value, 42);
    
    // Access name via get_name()
    EXPECT_EQ(loader.configs.group.get_name(), "group");
    
    // Direct access to name_ still works
    EXPECT_EQ(loader.configs.group.name_, "group");
}

TEST_F(ConfigsLoaderTest, DumpConfigsShowsAllValues) {
    ConfigsLoader<TestConfigs> loader;
    std::string dump = loader.dump_configs();
    
    EXPECT_NE(dump.find("--file=\"default.txt\""), std::string::npos);
    EXPECT_NE(dump.find("--count=10"), std::string::npos);
    EXPECT_NE(dump.find("--verbose=false"), std::string::npos);
}

TEST_F(ConfigsLoaderTest, DumpConfigsOnlyChangesShowsModified) {
    const char* argv[] = {"prog", "--count", "42"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    std::string dump = loader.dump_configs(SerializationFormat::CLI, true);
    
    EXPECT_NE(dump.find("--count=42"), std::string::npos);
    EXPECT_EQ(dump.find("--file"), std::string::npos);
    EXPECT_EQ(dump.find("--verbose"), std::string::npos);
}

TEST_F(ConfigsLoaderTest, DumpConfigsTomlShowsAllValues) {
    ConfigsLoader<TestConfigs> loader;
    std::string dump = loader.dump_configs(SerializationFormat::TOML);
    
    EXPECT_NE(dump.find("file = \"default.txt\""), std::string::npos);
    EXPECT_NE(dump.find("count = 10"), std::string::npos);
    EXPECT_NE(dump.find("verbose = false"), std::string::npos);
}

TEST_F(ConfigsLoaderTest, DumpConfigsTomlOnlyChangesShowsModified) {
    const char* argv[] = {"prog", "--count", "42"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    std::string dump = loader.dump_configs(SerializationFormat::TOML, true);
    
    EXPECT_NE(dump.find("count = 42"), std::string::npos);
    EXPECT_EQ(dump.find("file"), std::string::npos);
    EXPECT_EQ(dump.find("verbose"), std::string::npos);
}
