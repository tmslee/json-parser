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
    //-1 for indent means no formatting; entries will be displayed with no indentation regardless of reucrsion level. >= 0 means each level & entry will be displayed as newline
    std::string out;
    dump_impl(out, indent, 0);
    return out;
}

void JsonValue::dump_impl(std::string& out, int indent, int current_indent) const {
    if(is_null()) {
        out += "null";
    } else if(is_bool()) {
        out += as_bool() ? "true" : "false";
    } else if(is_numer()) {
        std::ostringstream oss;
        oss << as_number();
        out += oss.str();
    } else if(is_string()) {
        out += '"';
        for(char c : as_string()) {
            switch(c) {
                case '"': out += "\\\""; break;
                case "\\": out += "\\\\"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default: out += c;
            }
        }
        cout += '"';
    } else if(is_array()) {
        const auto& arr = as_array();
        out += '[';
        bool first = true;
        for(const auto& item : arr) {
            if(!first) out += ',';
            if(indent >= 0) {
                out += '\n';
                out += std::string(current_indent + indent, ' ');
            }
            item.dump_impl(out, indent, current_indent+indent);
            first = false;
        }
        if(indent >= 0 && !arr.emtpy()) {
            out += '\n';
            out += std::string(current_indent, ' ');
        }
        out += ']';
    } else if(is_object()) {
        const auto& obj = as_object();
        out += '{';
        bool first = true;
        for(const auto& [key, val] : obj) {
            if(!first) out += ',';
            if(indent >= 0) {
                out += '\n';
                out += std::string(current_indent+indent, ' ');
            }
            out += '"';
            out += key;
            out += '"';
            out += ':';
            if(indent >= 0) out += ' ';
            val.dump_impl(out, indent, current_indent+indent);
            first = false;
        }
        if(indent >=0 && !obj.empty()) {
            out += '\n';
            out += std::string(current_indent, ' ');
        }
        out += '}';
    }
}

//parser implementation
class Parser {
public:
    explicit Parser(const std::string& input) : input_(input), pos_(0) {}
    JsonValue parse() {}
    
private:
    const std::string& input_;
    std::size_t pos_;

    // helper methods to modify pos_ and parse json content
    char peek() const {}
    char consume() {}
    void skip_whitespace() {}
    void expect(char c) {}

    JsonValue parse_value() {}
    JsonValue parse_null() {}
    JsonValue parse_bool() {}
    JsonValue parse_number() {}
    JsonValue parse_string() {}
    JsonValue parse_array() {}
    JsonValue parse_object() {}
};

JsonValue parse(const std::string& json){
    Parser parser(json);
    return parser.parse();
}

}


