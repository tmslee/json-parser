#include "json_parser/json.hpp"
#include <cctype>
#include <sstream>

namespace json {
    // type checks --------------------------------------------------------------
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

    //accessors --------------------------------------------------------------
    /*
        std::get extracts value from std::variant
        can also use auto* valptr = std::get_if<T>(&value_); here
    */
    // const accessors
    bool JsonValue::as_bool() const {
        if(!is_bool()) throw std::runtime_error("not a bool");
        return std::get<JsonBool>(value_);
    }
    double JsonValue::as_number() const {
        if(!is_number()) throw std::runtime_error("not a number");
        return std::get<JsonNumber>(value_);
    }
    const std::string& JsonValue::as_string() const {
        if(!is_string()) throw std::runtime_error("not a string");
        return std::get<JsonString>(value_);
    }
    const JsonArray& JsonValue::as_array() const {
        if(!is_array()) throw std::runtime_error("not an array");
        return std::get<JsonArray>(value_);
    }
    const JsonObject& JsonValue::as_object() const {
        if(!is_object()) throw std::runtime_error("not an object");
        return std::get<JsonObject>(value_);
    }
    //mutable accessors
    JsonArray& JsonValue::as_array() {
        if(!is_array()) throw std::runtime_error("not an array");
        return std::get<JsonArray>(value_);
    }
    JsonObject& JsonValue::as_object() {
        if(!is_object()) throw std::runtime_error("not an object");
        return std::get<JsonObject>(value_);
    }
    //array & obj access
    const JsonValue& JsonValue::operator[](std::size_t index) const {
        return as_array().at(index);
    }
    JsonValue& JsonValue::operator[](std::size_t index) {
        return as_array().at(index);
    }
    const JsonValue& JsonValue::operator[](const std::string& key) const {
        // const access - [] access will create missing key. we must used .at(key)
        return as_object().at(key);
    }
    JsonValue& JsonValue::operator[](const std::string& key) {
        //mutable access - [] access & creation of new key is fine
        return as_object()[key];
    }

    // size ---------------------------------------------------------------------- 
    std::size_t JsonValue::size() const {
        if(is_array()) return as_array().size();
        if(is_object()) return as_object().size();
        throw std::runtime_error("size() only valid for arrays and objects");
    }

    //serialization --------------------------------------------------------------
    std::string JsonValue::dump(int indent = -1) const {
        
    }

    void JsonValue::dump_impl(std::string& out, int indent, int current_indent) const {

    }

}

//parser implementation
class Parser {

};
