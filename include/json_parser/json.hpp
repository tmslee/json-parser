#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <memory>

namespace json {

class JsonValue;

using JsonNull = std::nullptr_t;
using JsonBool = bool;
using JsonNumber = double;
using JsonString = std::string
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::unordered_map<std::string, JsonValue>;

class JsonValue {
public:
    /*
        notes on variants:
        - pros:
            type safe at compile time
            no heap allocation for variant itself
            no manual memory management
            std::visit for exhaustive handling
        - cons:
            size is largest member + tag (JsonObject is big)
            recursive type requires forward declaration tricks
            compile times can be slower, error msgs can be ugly

        alternatives:
        1. tagged union (C-style)
            - full control over memory layout, smaller size (ptr instead of inline storage)
            - faster compile time
            BUT
            - manual memory management (dtor, cpy, mv)
            - increased complexity (easy to mess up - read wrong union member = UB) + more boilerplate
        2. inheritance + polymorphism
            - familar pattern, easy to extend with new types, visitor pattern works naturally
            BUT
            - heap alloc for every value + virtual call overhead
            - dynamic_cast or visitor for type access & more indirection
        3. std::any
            - simple, can hold anything
            BUT
            - no compile time type checking
            - std::any_cast throws if wrong type, no exhaustive matching
            - slower (type erased)

        for production json libraries (nlohmann/json) they often used tagged unions for performance & control
    */

    using Value = std::variant<
        JsonNull, 
        JsonBool, 
        JsonNumber, 
        JsonString, 
        JsonArray, 
        JsonObject
    >;
    
    //constructors for each type
    JsonValue() : value_(nullptr) {}
    JsonValue(std::nullptr_t) : value_(nullptr) {}
    JsonValue(bool b) : value_(b) {}
    JsonValue(int n) : value_(static_cast<double>(n)) {}
    JsonValue(const char* s) : value_(std::string(s)) {}
    JsonValue(std::string s) : value_(std::move(s)) {}
    JsonValue(JsonArray arr) : value_(std::move(arr)) {}
    JsonValue(JsonObject obj) : value_(std::move(obj)) {}

    //type checks
    //noexcept because std::holds_alternative is noexcept (no alloc, no throw)
    [[nodiscard]] bool is_null() const noexcept;
    [[nodiscard]] bool is_bool() const noexcept;
    [[nodiscard]] bool is_number() const noexcept;
    [[nodiscard]] bool is_string() const noexcept;
    [[nodiscard]] bool is_array() const noexcept;
    [[nodiscard]] bool is_object() const noexcept;

    //accessors (throw if wrong type)
    [[nodiscard]] bool as_bool() const;
    [[nodiscard]] double as_number() const;
    [[nodiscard]] const std::string& as_string() const;
    [[nodiscard]] const JsonArray& as_array() const;
    [[nodiscard]] const JsonObject& as_object() const;
    //mutable accessors
    [[nodiscard]] JsonArray& as_array();
    [[nodiscard]] JsonObject& as_object();

    //array access
    [[nodiscard]] const JsonValue& operator[](std::size_t index) const
    [[nodiscard]] JsonValue& operator[](std::size_t index);

    //object access
    [[nodiscard]] const JsonValue& operator[](const std::string& key) const;
    [[nodiscard]] JsonValue& operator[](const std::string& key);

    // size (for arrays and objs)
    [[nodiscard]] std::size_t size() const;

    //serialize back to JSON string
    [[nodiscard]] std::string dump(int indent = -1) const;


private:  
    Value value_;
    void dump_impl(std::string& out, int indent, int current_indent) const;
};

// parser exception
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, std::size_t pos)
        : std::runtime_error(msg + " at position " + std::to_string(pos)), position_(pos) {}
        
    [[nodiscard]] std::size_t position() const {return position_;}
    
private:
    std::size_t position_;
};

// only expose public API of parser & hide implementation details in src file
[[nodiscard]] JsonValue parse(const std::string& json);

}

#endif