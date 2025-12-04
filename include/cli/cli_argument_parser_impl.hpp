#pragma once

inline ParsedArguments CliArgumentParser::parse(int argc, char* argv[]) {
    ParsedArguments result;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.empty() || arg[0] != '-') {
            continue;
        }

        if (arg == "--help" || arg == "-h") {
            result.has_help = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                result.help_filter = argv[i + 1];
                ++i;
            }
            continue;
        }

        if (arg == "--preset" || arg == "-p") {
            if (i + 1 < argc) {
                result.preset_path = argv[i + 1];
                ++i;
            }
            continue;
        }

        if (arg == "--save-config") {
            if (i + 1 < argc) {
                result.save_config_path = argv[i + 1];
                ++i;
            }
            continue;
        }

        std::string value;
        bool has_value = false;

        const size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            const std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                result.preset_path = arg.substr(equals_pos + 1);
                continue;
            }
            value = arg.substr(equals_pos + 1);
            arg = flag;
            has_value = true;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            value = argv[i + 1];
            has_value = true;
            ++i;
        }

        if (has_value) {
            result.flags[arg] = value;
        }
    }
    
    return result;
}
