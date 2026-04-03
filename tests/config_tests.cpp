#include "configs_loader.hpp"
#include <gtest/gtest.h>

using namespace lightshield::config;

TEST(ConfigTest, DefaultValueIsSet) {
    Config<int> field{.default_value = 42};
    EXPECT_EQ(field.value, 42);
}

TEST(ConfigTest, IsNotSetInitially) {
    Config<int> field{.default_value = 42};
    EXPECT_FALSE(field.is_set());
}

TEST(ConfigTest, SetValueMarksAsSet) {
    Config<int> field{.default_value = 42};
    EXPECT_TRUE(field.set_value(100));
    EXPECT_TRUE(field.is_set());
    EXPECT_EQ(field.value, 100);
}

TEST(ConfigTest, ResetRestoresDefault) {
    Config<int> field{.default_value = 42};
    field.set_value(100);
    field.reset();
    EXPECT_FALSE(field.is_set());
    EXPECT_EQ(field.value, 42);
}

TEST(ConfigTest, RequiredFieldIsRequired) {
    Config<int> field{.default_value = 42, .required = true};
    EXPECT_TRUE(field.is_required());
}

TEST(ConfigTest, OptionalFieldIsNotRequired) {
    Config<int> field{.default_value = 42, .required = false};
    EXPECT_FALSE(field.is_required());
}

TEST(ConfigTest, FlagsAreStored) {
    Config<int> field{.default_value = 42, .flags = {"--test", "-t"}};
    const auto& flags = field.flags;
    EXPECT_EQ(flags.size(), 2u);
    EXPECT_EQ(flags[0], "--test");
    EXPECT_EQ(flags[1], "-t");
}

TEST(ConfigTest, VerifierRejectsInvalidValue) {
    Config<int> field{
        .default_value = 42,
        .verifier = [](const int& val) { return val > 0; }
    };
    EXPECT_FALSE(field.set_value(-10));
    EXPECT_EQ(field.value, 42);
}

TEST(ConfigTest, VerifierAcceptsValidValue) {
    Config<int> field{
        .default_value = 42,
        .verifier = [](const int& val) { return val > 0; }
    };
    EXPECT_TRUE(field.set_value(100));
    EXPECT_EQ(field.value, 100);
}

TEST(ConfigTest, StringFieldWorks) {
    Config<std::string> field{.default_value = "default"};
    EXPECT_EQ(field.value, "default");
    field.set_value("changed");
    EXPECT_EQ(field.value, "changed");
}

TEST(ConfigTest, BoolFieldWorks) {
    Config<bool> field{.default_value = false};
    EXPECT_FALSE(field.value);
    field.set_value(true);
    EXPECT_TRUE(field.value);
}

TEST(ConfigTest, DoubleFieldWorks) {
    Config<double> field{.default_value = 3.14};
    EXPECT_DOUBLE_EQ(field.value, 3.14);
    field.set_value(2.71);
    EXPECT_DOUBLE_EQ(field.value, 2.71);
}

TEST(ConfigTest, MemoryFootprintIsSmallForBasicTypes) {
    // Ensuring EnumTraits (usually containing 2 std::function = 64-128 bytes) 
    // is empty for non-enum types. 
    // sizeof(Config<int>) should be well under 128 bytes.
    EXPECT_LT(sizeof(Config<int>), 128u);
}

enum class TestEnum { A, B };
TEST(ConfigTest, EnumHasTraits) {
    Config<TestEnum> field{};
    field.default_value = TestEnum::A;
    field.enum_traits.parser = [](const std::string&) { return TestEnum::A; };
    EXPECT_TRUE(field.enum_traits.parser);
    
    // Enum version should be significantly larger than int version due to std::function traits
    EXPECT_GT(sizeof(Config<TestEnum>), sizeof(Config<int>));
}

struct LayoutConfigs {
    Config<int> a{.default_value = 1};
    Config<int> b{.default_value = 2};
    REGISTER_CONFIG_FIELDS(a, b)
};

TEST(ConfigTest, MemoryLayoutIsStableAndDirect) {
    LayoutConfigs configs;
    
    // Use raw pointer manipulation to verify that Config storage is direct
    // and layout is stable (allows single ldr instruction in ASM)
    uintptr_t addr_a = reinterpret_cast<uintptr_t>(&configs.a.value);
    uintptr_t addr_b = reinterpret_cast<uintptr_t>(&configs.b.value);
    
    EXPECT_EQ(configs.a.value, 1);
    *reinterpret_cast<int*>(addr_a) = 42;
    EXPECT_EQ(configs.a.value, 42); // Value changed via raw pointer access
    
    EXPECT_GT(addr_b, addr_a); // Predictable struct ordering
}
