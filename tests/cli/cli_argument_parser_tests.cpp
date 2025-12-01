#include "cli/cli_argument_parser.hpp"
#include <gtest/gtest.h>

class CliArgumentParserTest : public ::testing::Test {};

TEST_F(CliArgumentParserTest, ParsesLongFlagWithSpace) {
    const char* argv[] = {"prog", "--file", "test.txt"};
    const ParsedArguments args = CliArgumentParser::parse(3, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("--file"), "test.txt");
}

TEST_F(CliArgumentParserTest, ParsesShortFlagWithSpace) {
    const char* argv[] = {"prog", "-f", "test.txt"};
    const ParsedArguments args = CliArgumentParser::parse(3, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("-f"), "test.txt");
}

TEST_F(CliArgumentParserTest, ParsesLongFlagWithEquals) {
    const char* argv[] = {"prog", "--file=test.txt"};
    const ParsedArguments args = CliArgumentParser::parse(2, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("--file"), "test.txt");
}

TEST_F(CliArgumentParserTest, ParsesMultipleArguments) {
    const char* argv[] = {"prog", "--file", "test.txt", "--count", "5", "--verbose", "true"};
    const ParsedArguments args = CliArgumentParser::parse(7, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("--file"), "test.txt");
    EXPECT_EQ(args.flags.at("--count"), "5");
    EXPECT_EQ(args.flags.at("--verbose"), "true");
}

TEST_F(CliArgumentParserTest, LastValueWinsForDuplicates) {
    const char* argv[] = {"prog", "--count", "5", "--count", "10"};
    const ParsedArguments args = CliArgumentParser::parse(5, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("--count"), "10");
}

TEST_F(CliArgumentParserTest, DetectsHelpFlag) {
    const char* argv[] = {"prog", "--help"};
    const ParsedArguments args = CliArgumentParser::parse(2, const_cast<char**>(argv));
    
    EXPECT_TRUE(args.has_help);
    EXPECT_EQ(args.help_filter, "");
}

TEST_F(CliArgumentParserTest, DetectsHelpWithFilter) {
    const char* argv[] = {"prog", "--help", "required"};
    const ParsedArguments args = CliArgumentParser::parse(3, const_cast<char**>(argv));
    
    EXPECT_TRUE(args.has_help);
    EXPECT_EQ(args.help_filter, "required");
}

TEST_F(CliArgumentParserTest, DetectsPresetPath) {
    const char* argv[] = {"prog", "--preset", "config.toml"};
    const ParsedArguments args = CliArgumentParser::parse(3, const_cast<char**>(argv));
    
    ASSERT_TRUE(args.preset_path.has_value());
    EXPECT_EQ(args.preset_path.value(), "config.toml");
}

TEST_F(CliArgumentParserTest, DetectsPresetPathWithEquals) {
    const char* argv[] = {"prog", "--preset=config.toml"};
    const ParsedArguments args = CliArgumentParser::parse(2, const_cast<char**>(argv));
    
    ASSERT_TRUE(args.preset_path.has_value());
    EXPECT_EQ(args.preset_path.value(), "config.toml");
}

TEST_F(CliArgumentParserTest, MixedFlagFormats) {
    const char* argv[] = {"prog", "-f", "test.txt", "--count=5", "--verbose", "true"};
    const ParsedArguments args = CliArgumentParser::parse(6, const_cast<char**>(argv));
    
    EXPECT_EQ(args.flags.at("-f"), "test.txt");
    EXPECT_EQ(args.flags.at("--count"), "5");
    EXPECT_EQ(args.flags.at("--verbose"), "true");
}
