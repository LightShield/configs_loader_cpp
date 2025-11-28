#include "configs_loader.hpp"
#include <gtest/gtest.h>

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

TEST(ConfigsLoaderTest, GenerateHelpIncludesAllFields) {
    ConfigsLoader<TestConfigs> loader;
    std::string help = loader.generate_help("test_prog");
    
    EXPECT_NE(help.find("test_prog"), std::string::npos);
    EXPECT_NE(help.find("--file"), std::string::npos);
    EXPECT_NE(help.find("-f"), std::string::npos);
    EXPECT_NE(help.find("--count"), std::string::npos);
    EXPECT_NE(help.find("-c"), std::string::npos);
    EXPECT_NE(help.find("--verbose"), std::string::npos);
    EXPECT_NE(help.find("-v"), std::string::npos);
    EXPECT_NE(help.find("--preset"), std::string::npos);
}

TEST(ConfigsLoaderTest, GenerateHelpShowsDefaults) {
    ConfigsLoader<TestConfigs> loader;
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("default: \"default.txt\""), std::string::npos);
    EXPECT_NE(help.find("default: 10"), std::string::npos);
    EXPECT_NE(help.find("default: false"), std::string::npos);
}

TEST(ConfigsLoaderTest, GenerateHelpMarksRequired) {
    struct RequiredConfigs {
        Config<std::string> required_field{
            .default_value = "",
            .flags = {"--required"},
            .required = true,
            .description = "A required configuration field"
        };
        Config<std::string> optional_field{
            .default_value = "opt",
            .flags = {"--optional"},
            .description = "An optional configuration field"
        };
        REGISTER_CONFIG_FIELDS(required_field, optional_field)
    };
    
    ConfigsLoader<RequiredConfigs> loader;
    std::string help = loader.generate_help();
    
    // Find the Options section
    size_t options_start = help.find("Options:");
    ASSERT_NE(options_start, std::string::npos);
    
    // Search only within the Options section
    std::string options_section = help.substr(options_start);
    
    // Check that [Required] and --required both appear
    EXPECT_NE(options_section.find("[Required]"), std::string::npos);
    EXPECT_NE(options_section.find("--required"), std::string::npos);
    
    // Check ordering within options section
    size_t help_pos = options_section.find("--help");
    size_t preset_pos = options_section.find("--preset");
    size_t required_marker_pos = options_section.find("[Required]");
    size_t optional_pos = options_section.find("--optional");
    
    EXPECT_LT(help_pos, preset_pos);
    EXPECT_LT(preset_pos, required_marker_pos);
    EXPECT_LT(required_marker_pos, optional_pos);
    
    // Check descriptions appear
    EXPECT_NE(help.find("A required configuration field"), std::string::npos);
    EXPECT_NE(help.find("An optional configuration field"), std::string::npos);
}

TEST(ConfigsLoaderTest, GenerateHelpShowsDefaultDescriptionWhenMissing) {
    struct NoDescConfigs {
        Config<std::string> field{
            .default_value = "test",
            .flags = {"--field"}
        };
        REGISTER_CONFIG_FIELDS(field)
    };
    
    ConfigsLoader<NoDescConfigs> loader;
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("No description provided for this config"), std::string::npos);
}
