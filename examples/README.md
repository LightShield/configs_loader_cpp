# ConfigsLoader Examples

This directory contains examples demonstrating different usage patterns of the ConfigsLoader library.

## Examples Overview

### 1. basic_example
**Purpose**: Demonstrates core features with hardcoded arguments

Shows:
- Config struct definition with designated initializers
- Required and optional fields
- Direct value access (no getter overhead)
- Hardcoded arguments for reproducible demonstration

Run: `./build/examples/basic/basic_example`

### 2. cli_example
**Purpose**: Real command-line interface usage

Shows:
- Accepting actual argc/argv from command line
- Automatic --help handling (no manual code needed)
- Error handling for missing required fields
- Multiple flag formats (--flag value, -f value)

Run: 
```bash
./build/examples/cli/cli_example --help
./build/examples/cli/cli_example --input data.txt --output result.txt --verbose true
```

### 3. help_example
**Purpose**: Demonstrates help generation customization

Shows:
- Generating help text programmatically
- Customizing help width for different terminal sizes
- Column-aligned formatting
- Text wrapping behavior

Run: `./build/examples/help/help_example`

### 4. global_example
**Purpose**: Multi-file configuration access pattern

Shows:
- Global config loader pattern
- Safe accessor function with initialization check
- Accessing configs from multiple source files
- Unsafe direct access for performance-critical code

Run: `./build/examples/global/global_example --file config.txt --log-level 3`

## Key Features Demonstrated

- **Automatic --help**: ConfigsLoader handles --help/-h automatically, printing help and exiting
- **Type safety**: Compile-time type checking for config values
- **Zero overhead**: Direct value access without getter functions
- **Flexible CLI parsing**: Supports --flag value, -f value, --flag=value formats
- **Column-aligned help**: Professional-looking help output with proper alignment
- **Text wrapping**: Automatic wrapping of long descriptions with indentation
- **Required fields**: Validation of required configuration at initialization
