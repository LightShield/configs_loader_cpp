#include "serialization/toml_deserializer.hpp"
#include <gtest/gtest.h>
#include <fstream>

#ifdef CONFIGS_LOADER_ENABLE_TOML

// Learning tests for toml++ library
TEST(TomlLearningTest, ParseSimpleFile) {
    // Create test TOML file
    std::ofstream out("/tmp/test.toml");
    out << "name = \"test\"\n";
    out << "count = 42\n";
    out << "enabled = true\n";
    out << "ratio = 3.14\n";
    out.close();
    
    auto tbl = toml::parse_file("/tmp/test.toml");
    
    EXPECT_EQ(tbl["name"].value<std::string>(), "test");
    EXPECT_EQ(tbl["count"].value<int64_t>(), 42);
    EXPECT_EQ(tbl["enabled"].value<bool>(), true);
    EXPECT_DOUBLE_EQ(tbl["ratio"].value<double>().value(), 3.14);
}

TEST(TomlLearningTest, MissingKeyReturnsNullopt) {
    std::ofstream out("/tmp/test2.toml");
    out << "name = \"test\"\n";
    out.close();
    
    auto tbl = toml::parse_file("/tmp/test2.toml");
    
    EXPECT_FALSE(tbl["missing"].value<std::string>().has_value());
}

// Tests for our TomlDeserializer wrapper
TEST(TomlDeserializerTest, ParseFileSucceeds) {
    std::ofstream out("/tmp/config.toml");
    out << "file = \"data.txt\"\n";
    out << "log-level = 3\n";
    out << "verbose = true\n";
    out.close();
    
    TomlDeserializer parser;
    EXPECT_NO_THROW(parser.parse_file("/tmp/config.toml"));
}

TEST(TomlDeserializerTest, GetStringWorks) {
    std::ofstream out("/tmp/string_test.toml");
    out << "name = \"hello\"\n";
    out.close();
    
    TomlDeserializer parser;
    parser.parse_file("/tmp/string_test.toml");
    
    auto val = parser.get_string("name");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "hello");
}

TEST(TomlDeserializerTest, GetIntWorks) {
    std::ofstream out("/tmp/int_test.toml");
    out << "count = 123\n";
    out.close();
    
    TomlDeserializer parser;
    parser.parse_file("/tmp/int_test.toml");
    
    auto val = parser.get_int("count");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 123);
}

TEST(TomlDeserializerTest, GetBoolWorks) {
    std::ofstream out("/tmp/bool_test.toml");
    out << "enabled = false\n";
    out.close();
    
    TomlDeserializer parser;
    parser.parse_file("/tmp/bool_test.toml");
    
    auto val = parser.get_bool("enabled");
    ASSERT_TRUE(val.has_value());
    EXPECT_FALSE(*val);
}

TEST(TomlDeserializerTest, GetDoubleWorks) {
    std::ofstream out("/tmp/double_test.toml");
    out << "ratio = 2.5\n";
    out.close();
    
    TomlDeserializer parser;
    parser.parse_file("/tmp/double_test.toml");
    
    auto val = parser.get_double("ratio");
    ASSERT_TRUE(val.has_value());
    EXPECT_DOUBLE_EQ(*val, 2.5);
}

TEST(TomlDeserializerTest, MissingKeyReturnsNullopt) {
    std::ofstream out("/tmp/missing_test.toml");
    out << "name = \"test\"\n";
    out.close();
    
    TomlDeserializer parser;
    parser.parse_file("/tmp/missing_test.toml");
    
    EXPECT_FALSE(parser.get_string("missing").has_value());
    EXPECT_FALSE(parser.get_int("missing").has_value());
}

TEST(TomlDeserializerTest, InvalidFileThrows) {
    TomlDeserializer parser;
    EXPECT_THROW(parser.parse_file("/nonexistent/file.toml"), std::runtime_error);
}

TEST(TomlDeserializerTest, MalformedTomlThrows) {
    std::ofstream out("/tmp/bad.toml");
    out << "this is not valid toml [[[[\n";
    out.close();
    
    TomlDeserializer parser;
    EXPECT_THROW(parser.parse_file("/tmp/bad.toml"), std::runtime_error);
}

#endif // CONFIGS_LOADER_ENABLE_TOML
