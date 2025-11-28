#include "config_field.h"
#include <gtest/gtest.h>

TEST(ConfigFieldTest, DefaultValueIsSet) {
    ConfigField<int> field{ConfigField<int>::Metadata{.default_value = 42}};
    EXPECT_EQ(field.value(), 42);
}

TEST(ConfigFieldTest, IsNotSetInitially) {
    ConfigField<int> field{ConfigField<int>::Metadata{.default_value = 42}};
    EXPECT_FALSE(field.is_set());
}

TEST(ConfigFieldTest, SetValueMarksAsSet) {
    ConfigField<int> field{ConfigField<int>::Metadata{.default_value = 42}};
    EXPECT_TRUE(field.set_value(100));
    EXPECT_TRUE(field.is_set());
    EXPECT_EQ(field.value(), 100);
}

TEST(ConfigFieldTest, ResetRestoresDefault) {
    ConfigField<int> field{ConfigField<int>::Metadata{.default_value = 42}};
    field.set_value(100);
    field.reset();
    EXPECT_FALSE(field.is_set());
    EXPECT_EQ(field.value(), 42);
}

TEST(ConfigFieldTest, RequiredFieldIsRequired) {
    ConfigField<int> field{ConfigField<int>::Metadata{
        .default_value = 42,
        .required = true
    }};
    EXPECT_TRUE(field.is_required());
}

TEST(ConfigFieldTest, OptionalFieldIsNotRequired) {
    ConfigField<int> field{ConfigField<int>::Metadata{
        .default_value = 42,
        .required = false
    }};
    EXPECT_FALSE(field.is_required());
}

TEST(ConfigFieldTest, FlagsAreStored) {
    ConfigField<int> field{ConfigField<int>::Metadata{
        .default_value = 42,
        .flags = {"--test", "-t"}
    }};
    const auto& flags = field.flags();
    EXPECT_EQ(flags.size(), 2u);
    EXPECT_EQ(flags[0], "--test");
    EXPECT_EQ(flags[1], "-t");
}

TEST(ConfigFieldTest, VerifierRejectsInvalidValue) {
    ConfigField<int> field{ConfigField<int>::Metadata{
        .default_value = 42,
        .verifier = [](const int& val) { return val > 0; }
    }};
    EXPECT_FALSE(field.set_value(-10));
    EXPECT_EQ(field.value(), 42);
}

TEST(ConfigFieldTest, VerifierAcceptsValidValue) {
    ConfigField<int> field{ConfigField<int>::Metadata{
        .default_value = 42,
        .verifier = [](const int& val) { return val > 0; }
    }};
    EXPECT_TRUE(field.set_value(100));
    EXPECT_EQ(field.value(), 100);
}

TEST(ConfigFieldTest, StringFieldWorks) {
    ConfigField<std::string> field{ConfigField<std::string>::Metadata{
        .default_value = "default"
    }};
    EXPECT_EQ(field.value(), "default");
    field.set_value("changed");
    EXPECT_EQ(field.value(), "changed");
}

TEST(ConfigFieldTest, BoolFieldWorks) {
    ConfigField<bool> field{ConfigField<bool>::Metadata{
        .default_value = false
    }};
    EXPECT_FALSE(field.value());
    field.set_value(true);
    EXPECT_TRUE(field.value());
}

TEST(ConfigFieldTest, DoubleFieldWorks) {
    ConfigField<double> field{ConfigField<double>::Metadata{
        .default_value = 3.14
    }};
    EXPECT_DOUBLE_EQ(field.value(), 3.14);
    field.set_value(2.71);
    EXPECT_DOUBLE_EQ(field.value(), 2.71);
}
