# Examples

## Building Examples

From the project root:

```bash
mkdir build && cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

Examples will be in `build/examples/`.

## Available Examples

### basic/
Single-file usage pattern. Shows how to use ConfigsLoader in a simple application.

**Run:**
```bash
./examples/basic_example --file input.txt --log-level 3
```

### global/
Multi-file usage pattern with extern declaration. Shows how to access configs across multiple files without passing them around.

**Run:**
```bash
./examples/global_example --file data.txt --log-level 5
```

## Pattern Comparison

**Basic Pattern:** Use when configs are only needed in one file.

**Global Pattern:** Use when configs need to be accessed across multiple files. Provides both safe (`GetConfigs()`) and unsafe (direct `g_config_loader.configs`) access.
