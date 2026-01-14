# JSON Parser
A C++20 JSON parser implementation with `std::variant` based value representation.

## Features
- Full JSON parsing (null, bool, number, string, array, object)
- Recursive descent parser
- Unicode escape support (`\uXXXX`)
- Serialization with optional pretty-printing
- Type-safe value access
- No external dependencies

## Requirements
- C++20 compiler (GCC 10+, Clang 11+)
- CMake 3.20+ (for building tests)

## Usage
```cpp
#include <json_parser/json.hpp>
#include <iostream>

int main() {
    // Parse JSON
    auto val = json::parse(R"({
        "name": "Alice",
        "age": 30,
        "scores": [95, 87, 92]
    })");

    // Access values
    std::cout << val["name"].as_string() << "\n";
    std::cout << val["age"].as_number() << "\n";
    std::cout << val["scores"][0].as_number() << "\n";

    // Modify
    val["age"] = 31;

    // Serialize
    std::cout << val.dump(2) << "\n";  // Pretty print with 2-space indent
    std::cout << val.dump() << "\n";   // Compact (no formatting)
}
```

## API

### Parsing
```cpp
json::JsonValue parse(const std::string& json);
```
Throws `json::ParseError` on invalid input.

### Type Checks
```cpp
bool is_null() const noexcept;
bool is_bool() const noexcept;
bool is_number() const noexcept;
bool is_string() const noexcept;
bool is_array() const noexcept;
bool is_object() const noexcept;
```

### Accessors
```cpp
bool as_bool() const;
double as_number() const;
const std::string& as_string() const;
const JsonArray& as_array() const;
const JsonObject& as_object() const;
```
All throw `std::runtime_error` if wrong type.

### Element Access
```cpp
// Array access
JsonValue& operator[](std::size_t index);

// Object access
JsonValue& operator[](const std::string& key);
```

### Serialization
```cpp
std::string dump(int indent = -1) const;
```

- `indent = -1`: Compact, single line
- `indent >= 0`: Pretty print with newlines and specified indentation

## Building Tests
```bash
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## With Sanitizers
```bash
cmake -B build -DENABLE_SANITIZERS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Design Notes

### Why `std::variant`
The `JsonValue` class uses `std::variant` to hold one of six possible types:
**Pros:**
- Type-safe at compile time
- No heap allocation for the variant itself
- No manual memory management
- `std::visit` enables exhaustive handling
**Cons:**
- Size is largest member + tag (JsonObject is large)
- Recursive types require forward declaration tricks
- Compile times can be slower
- Error messages can be verbose

**Alternatives considered:**
1. **Tagged union (C-style)**: Full control over memory layout, smaller size, faster compile times. But requires manual memory management, more boilerplate, and reading the wrong union member is undefined behavior.
2. **Inheritance + polymorphism**: Familiar OOP pattern, easy to extend, visitor pattern works naturally. But requires heap allocation for every value, virtual call overhead, and `dynamic_cast` for type access.
3. **`std::any`**: Simple, can hold anything. But no compile-time type checking, `std::any_cast` throws on wrong type, no exhaustive matching, and slower due to type erasure.

Production JSON libraries like nlohmann/json often use tagged unions for maximum performance and control.

### Recursive Type Problem
The variant contains `JsonArray` (`std::vector<JsonValue>`) and `JsonObject` (`std::unordered_map<std::string, JsonValue>`), which themselves contain `JsonValue`. This creates a circular dependency:
- `std::vector<JsonValue>` needs to know `sizeof(JsonValue)` to allocate storage
- `std::unordered_map` needs complete `JsonValue` for its internal `std::pair`
- `std::variant` needs complete types to determine its size
- But `JsonValue` isn't complete until after the closing brace
Attempting `std::variant<..., JsonArray, JsonObject>` directly would fail with an incomplete type error.

**Solution:** Wrap containers in `std::unique_ptr`:
```cpp
using Value = std::variant
    JsonNull,
    JsonBool,
    JsonNumber,
    JsonString,
    std::unique_ptr<JsonArray>,
    std::unique_ptr<JsonObject>
>;
```
This works because:
- `std::unique_ptr<T>` is just a pointer internally; the compiler knows its size (8 bytes on 64-bit) without knowing `sizeof(T)`
- `T` only needs to be complete when dereferencing (`->`, `*`) or when the destructor runs
- Both happen in the `.cpp` file where `JsonValue` is complete
**Tradeoff:** Arrays and objects are now heap-allocated. Scalars (null, bool, number, string) remain inline in the variant.

### Copy Semantics

Because `std::unique_ptr` is move-only, the default copy constructor won't work. We implement explicit copy:
```cpp
JsonValue::JsonValue(const JsonValue& other) {
    if (other.is_array()) {
        value_ = std::make_unique<JsonArray>(other.as_array());
    } else if (other.is_object()) {
        value_ = std::make_unique<JsonObject>(other.as_object());
    }
    // ... handle other types
}
```
Copy assignment uses the copy-and-swap idiom for exception safety.
Move operations use compiler defaults since `std::unique_ptr` handles them correctly.

### Parser Architecture
The parser is implemented as a private class in the source file, exposing only:
```cpp
JsonValue parse(const std::string& json);
```
This hides implementation details (position tracking, helper methods) from users and keeps the public API clean.
The parser uses recursive descent, where each JSON type has a dedicated parse function:
```
parse_value()
    ├── parse_null()
    ├── parse_bool()
    ├── parse_number()
    ├── parse_string()
    ├── parse_array()   → calls parse_value() recursively
    └── parse_object()  → calls parse_value() recursively
```

### Number Parsing
JSON number rules are strict:
**Valid:**
- `42`, `-17`, `0` (integers)
- `3.14`, `-0.5`, `0.0` (decimals)
- `1e10`, `2E5`, `1e-3`, `3.14e+2` (exponents)

**Invalid:**
- `.5` (no leading digit)
- `5.` (no digit after decimal)
- `+5` (no leading plus)
- `05`, `00.5` (no leading zeros except `0` itself)
- `0x1F` (no hex)
- `NaN`, `Infinity` (not allowed)

### Unicode Support
The parser handles `\uXXXX` escapes for the Basic Multilingual Plane (U+0000 to U+FFFF) with UTF-8 encoding:
- Codepoints < 0x80: Single byte
- Codepoints < 0x800: Two bytes
- Codepoints < 0x10000: Three bytes
Surrogate pairs for characters above U+FFFF (like emoji) are not currently supported.

### Const vs Mutable Access
Object access behaves differently based on const-ness:
```cpp
// Const access: throws if key missing
const JsonValue& operator[](const std::string& key) const {
    return as_object().at(key);  // .at() throws std::out_of_range
}
// Mutable access: creates key if missing
JsonValue& operator[](const std::string& key) {
    return as_object()[key];  // [] inserts default value
}
```
This mirrors `std::map` behavior and allows natural assignment:
```cpp
val["new_key"] = 42;  // Creates key
```

## License
MIT