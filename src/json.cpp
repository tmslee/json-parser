#include "json_parser/json.hpp"
#include <cctype>
#include <sstream>

namespace json {
    // type checks
    bool JsonValue::is_null() const noexcept {
        return std::holds_alternative<JsonNull>(value_);
    }
    bool JsonValue::is_bool() const noexcept {
        return std::holds_alternative<JsonBool>(value_);
    }
    bool JsonValue::is_number() const noexcept {
        return std::holds_alternative<JsonNumber>(value_);
    }
    bool JsonValue::is_string() const noexcept {
        return std::holds_alternative<JsonString>(value_);
    }
    bool JsonValue::is_array() const noexcept {
        return std::holds_alternative<JsonArray>(value_);
    }
    bool JsonValue::is_object() const noexcept {
        return std::holds_alternative<JsonObject>(value_);
    }

    //accessors
    bool JsonValue::as_bool() const;
    double JsonValue::as_number() const;
    const std::string& JsonValue::as_string() const;
    const JsonArray& JsonValue::as_array() const;
    const JsonObject& JsonValue::as_object() const;
    JsonArray& JsonValue::as_array();
    JsonObject& JsonValue::as_object();

    const JsonValue& JsonValue::operator[](std::size_t index) const
    JsonValue& JsonValue::operator[](std::size_t index);

    const JsonValue& JsonValue::operator[](const std::string& key) const;
    JsonValue& JsonValue::operator[](const std::string& key);


    // size
    std::size_t JsonValue::size() const;

    //serialization
    std::string JsonValue::dump(int indent = -1) const;

    void JsonValue::dump_impl(std::string& out, int indent, int current_indent) const {
        
    }

}

//parser implementation
class Parser {

};
