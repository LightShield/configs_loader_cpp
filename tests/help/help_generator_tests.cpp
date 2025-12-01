#include "configs_loader.hpp"
#include <gtest/gtest.h>

class HelpGeneratorTest : public ::testing::Test {
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

    ConfigsLoader<TestConfigs> loader;

    void SetUp() override {
        loader.help_format.use_colors = false;
        loader.help_format.enable_interactive = false;
    }
};

TEST_F(HelpGeneratorTest, IncludesAllFields) {
    loader.help_format.program_name = "test_prog";
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("test_prog"), std::string::npos);
    EXPECT_NE(help.find("--file"), std::string::npos);
    EXPECT_NE(help.find("-f"), std::string::npos);
    EXPECT_NE(help.find("--count"), std::string::npos);
    EXPECT_NE(help.find("-c"), std::string::npos);
    EXPECT_NE(help.find("--verbose"), std::string::npos);
    EXPECT_NE(help.find("-v"), std::string::npos);
    EXPECT_NE(help.find("--preset"), std::string::npos);
}

TEST_F(HelpGeneratorTest, ShowsDefaults) {
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("default: \"default.txt\""), std::string::npos);
    EXPECT_NE(help.find("default: 10"), std::string::npos);
    EXPECT_NE(help.find("default: false"), std::string::npos);
}

TEST_F(HelpGeneratorTest, MarksRequired) {
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
    loader.help_format.use_colors = false;
    loader.help_format.enable_interactive = false;
    std::string help = loader.generate_help();
    
    size_t options_start = help.find("Options:");
    ASSERT_NE(options_start, std::string::npos);
    
    std::string options_section = help.substr(options_start);
    
    EXPECT_NE(options_section.find("[Required]"), std::string::npos);
    EXPECT_NE(options_section.find("--required"), std::string::npos);
    
    size_t help_pos = options_section.find("--help");
    size_t preset_pos = options_section.find("--preset");
    size_t required_marker_pos = options_section.find("[Required]");
    size_t optional_pos = options_section.find("--optional");
    
    EXPECT_LT(help_pos, preset_pos);
    EXPECT_LT(preset_pos, required_marker_pos);
    EXPECT_LT(required_marker_pos, optional_pos);
    
    EXPECT_NE(help.find("A required configuration field"), std::string::npos);
    EXPECT_NE(help.find("An optional configuration field"), std::string::npos);
}

TEST_F(HelpGeneratorTest, ShowsDefaultDescriptionWhenMissing) {
    struct NoDescConfigs {
        Config<std::string> field{
            .default_value = "test",
            .flags = {"--field"}
        };
        REGISTER_CONFIG_FIELDS(field)
    };
    
    ConfigsLoader<NoDescConfigs> loader;
    loader.help_format.use_colors = false;
    loader.help_format.enable_interactive = false;
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("No description provided for this config"), std::string::npos);
}

TEST_F(HelpGeneratorTest, ShowsCurrentValueWhenDifferentFromDefault) {
    const char* argv[] = {"prog", "--count", "42"};
    ConfigsLoader<TestConfigs> loader(3, const_cast<char**>(argv));
    loader.help_format.use_colors = false;
    loader.help_format.enable_interactive = false;
    loader.help_format.show_current_values = true;
    
    std::string help = loader.generate_help();
    
    EXPECT_NE(help.find("current: 42"), std::string::npos);
    EXPECT_NE(help.find("default: 10"), std::string::npos);
}

TEST_F(HelpGeneratorTest, DoesNotShowCurrentWhenSameAsDefault) {
    ConfigsLoader<TestConfigs> loader;
    loader.help_format.use_colors = false;
    loader.help_format.enable_interactive = false;
    loader.help_format.show_current_values = true;
    
    std::string help = loader.generate_help();
    
    EXPECT_EQ(help.find("current:"), std::string::npos);
    EXPECT_NE(help.find("default: 10"), std::string::npos);
}

TEST_F(HelpGeneratorTest, AcceptsCustomFormat) {
    ConfigsLoader<TestConfigs> loader;
    
    HelpFormat custom_format{
        .program_name = "custom_prog",
        .use_colors = false,
        .enable_interactive = false
    };
    
    std::string help = loader.generate_help("", std::cref(custom_format));
    
    EXPECT_NE(help.find("custom_prog"), std::string::npos);
    EXPECT_EQ(help.find("test_prog"), std::string::npos);
}

