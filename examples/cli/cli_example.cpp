#include "configs_loader.hpp"
#include <iostream>

struct CliConfigs {
    Config<std::string> input{
        .default_value = "input.txt",
        .flags = {"--input", "-i"},
        .required = true,
        .description = "Input file path"
    };
    
    Config<std::string> output{
        .default_value = "output.txt",
        .flags = {"--output", "-o"},
        .description = "Output file path"
    };
    
    Config<bool> verbose{
        .default_value = false,
        .flags = {"--verbose", "-v"},
        .description = "Enable verbose output"
    };

    REGISTER_CONFIG_FIELDS(input, output, verbose)
};

int main(int argc, char* argv[]) {
    // This example uses real command-line arguments
    // Try running: ./cli_example --input data.txt --output result.txt --verbose true
    // Or: ./cli_example --help
    
    ConfigsLoader<CliConfigs> loader;
    
    try {
        loader.Init(argc, argv);  // --help is handled automatically
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Use --help for usage information\n";
        return 1;
    }
    
    // Process with the loaded configuration
    std::cout << "Processing:\n";
    std::cout << "  Input:  " << loader.configs.input.value << "\n";
    std::cout << "  Output: " << loader.configs.output.value << "\n";
    std::cout << "  Verbose: " << (loader.configs.verbose.value ? "enabled" : "disabled") << "\n";
    
    return 0;
}
