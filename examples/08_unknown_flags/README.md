# Example 08: Unknown Flags Handling

Demonstrates how to control behavior when unknown flags are provided.

## Unknown Flag Behaviors

- **Error** (default) - Fail with error message, catches typos
- **Warn** - Print warning, continue execution
- **Ignore** - Silent, for flexible config mixing

## Usage

```cpp
ConfigsLoader<MyConfig> loader;
loader.unknown_flag_behavior = UnknownFlagBehavior::Warn;  // or Error, or Ignore
loader.init(argc, argv);
```

## Running

```bash
# Error behavior (default) - fails on unknown flag
./unknown_flags_example --unknown test

# Warn behavior - shows warning, continues
./unknown_flags_example --unknown test

# Ignore behavior - silent
./unknown_flags_example --unknown test
```

## When to Use Each

- **Error**: Production apps (catch typos, strict validation)
- **Warn**: Development/debugging (see what's ignored)
- **Ignore**: Config file mixing, forward compatibility
