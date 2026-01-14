#include "json_parser/json.hpp"
#include <cctype>
#include <sstream>

namespace json {

JsonValue::JsonValue(const JsonValue& other) {
    if(other.is_null()) {
        value_ = nullptr;
    } else if(other.is_bool()) {
        value_ = other.as_bool();
    } else if(other.is_number()) {
        value_ = other.as_number();
    } else if(other.is_string()) {
        value_ = other.as_string();
    } else if(other.is_array()) {
        value_ = std::make_unique<JsonArray>(other.as_array());
    } else if(other.is_object()) {
        value_ = std::make_unique<JsonObject>(other.as_object());
    }
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
    if(this != &other) {
        JsonValue temp(other);
        std::swap(value_, temp.value_);
    }
    return *this;
}

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
    return std::holds_alternative<std::unique_ptr<JsonArray>>(value_);
}
bool JsonValue::is_object() const noexcept {
    return std::holds_alternative<std::unique_ptr<JsonObject>>(value_);
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
    return *std::get<std::unique_ptr<JsonArray>>(value_);
}
const JsonObject& JsonValue::as_object() const {
    if(!is_object()) throw std::runtime_error("not an object");
    return *std::get<std::unique_ptr<JsonObject>>(value_);
}
//mutable accessors
JsonArray& JsonValue::as_array() {
    if(!is_array()) throw std::runtime_error("not an array");
    return *std::get<std::unique_ptr<JsonArray>>(value_);
}
JsonObject& JsonValue::as_object() {
    if(!is_object()) throw std::runtime_error("not an object");
    return *std::get<std::unique_ptr<JsonObject>>(value_);
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
std::string JsonValue::dump(int indent) const {
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
    } else if(is_number()) {
        std::ostringstream oss;
        oss << as_number();
        out += oss.str();
    } else if(is_string()) {
        out += '"';
        for(char c : as_string()) {
            switch(c) {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default: out += c;
            }
        }
        out += '"';
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
        if(indent >= 0 && !arr.empty()) {
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
    JsonValue parse() {
        skip_whitespace();
        auto value = parse_value();
        skip_whitespace();
        if(pos_ != input_.size()) {
            throw ParseError("unexpected characters after JSON", pos_);
        }
        return value;
    }

private:
    const std::string& input_;
    std::size_t pos_;

    // helper methods to modify pos_ and parse json content
    char peek() const {
        if(pos_ >= input_.size()) return '\0';
        return input_[pos_];
    }
    char consume() {
        if(pos_ >= input_.size()) {
            throw ParseError("unexpected end of input", pos_);
        }
        return input_[pos_++];
    }
    void skip_whitespace() {
        while(pos_ < input_.size() && std::isspace(input_[pos_])) {
            ++pos_;
        }
    }
    void expect(char c) {
        if(consume() != c) {
            throw ParseError(std::string("expected '") + c + "'", pos_-1);
        }
    }

    JsonValue parse_value() {
        skip_whitespace();
        char c = peek();
        if(c == 'n') return parse_null();
        if(c=='t' || c=='f') return parse_bool();
        if(c=='"') return parse_string();
        if(c=='[') return parse_array();
        if(c=='{') return parse_object();
        if(c=='-' || std::isdigit(c)) return parse_number();
        throw ParseError("unexpected character", pos_);
    }

    JsonValue parse_null() {
        if(input_.substr(pos_, 4) == "null") {
            pos_+= 4;
            return JsonValue(nullptr);
        }
        throw ParseError("expected 'null'", pos_);
    }

    JsonValue parse_bool() {
        if(input_.substr(pos_, 4) == "true") {
            pos_+=4;
            return JsonValue(true);
        }
        if(input_.substr(pos_, 5) == "false") {
            pos_+=5;
            return JsonValue(false);
        }
        throw ParseError("expected 'true' or 'false'", pos_);
    }

    JsonValue parse_number() {
        /*
        note on valid numbers in json:
            integer:        42, -17, 0
            decimal:        3.14, -0.5, 0.0
            exponent:       1e10, 2E5, 1e-3, 3.14e+2
            negative exp:   1e-10, 5E-3
            combined:       -3.14e-10
        invalid:
            .5              (no leading digit)
            -.5             (no leading digit)
            5.              (no digit after decimal)
            +5              (no leading plus)
            05              (no leading zeros except 0 itself)
            00.5            (no leading zeros)
            0x1F            (no hex)
            NaN             (not allowed)
            Infinity        (not allowed)
        */

        std::size_t start = pos_;
        if(peek() == '-') ++pos_; //leading neg sign

        //this handles all pre-decimal point & exponents
        if(peek() == '0') {
            /*
                if we lead with zero, it must be:
                1. end of the value
                2. followed by decimal
                3. followed by exponent
                it CANNOT be followed by more digits
            */
            ++pos_;
        } else if(std::isdigit(peek())) {
            // only check trailing digits if leading digit is NOT zero
            while(std::isdigit(peek())) ++pos_;
        } else {
            // numeric value cannot lead with a non digit
            throw ParseError("invalid number", pos_);
        }
        
        //handle decimal point (all decimal must be followed by string of digits)
        if(peek() == '.') {
            ++pos_;
            if(!std::isdigit(peek())) {
                throw ParseError("invalid number", pos_);
            }
            while(std::isdigit(peek())) ++pos_;
        }

        //handle exponents (all exponent must be followed by +/- then a string of digits)
        if(peek() == 'e' || peek() == 'E') {
            ++pos_;
            if(peek() == '+' || peek() == '-') ++pos_;
            if(!std::isdigit(peek())) {
                throw ParseError("invalid number", pos_);
            }
            while(std::isdigit(peek())) ++pos_;
        }

        std::string num_str = input_.substr(start, pos_-start);
        return JsonValue(std::stod(num_str));
    }

    JsonValue parse_string() {
        expect('"');
        std::string str;
        while(peek() != '"') {
            if(peek() == '\0'){
                throw ParseError("unterminated string", pos_);
            }
            if(peek() == '\\') {
                //deal with escape characters
                ++pos_;
                char escape = consume();
                switch(escape) {
                    case '"': str += '"'; break;
                    case '\\': str += '\\'; break;
                    case '/' : str += '/'; break;
                    case 'n' : str += '\n'; break;
                    case 'r' : str += '\r'; break;
                    case 't' : str += '\t'; break;
                    case 'b' : str += '\b'; break;
                    case 'f' : str += '\f'; break;
                    case 'u': {
                        //unicode escape: \uXXXX
                        //our implementation only handles basic multilingual plane. characters above 0xFFFF requires surrogate pairs, which we dont handle.
                        std::string hex;
                        for(int i=0; i<4; ++i) {
                            if(!std::isxdigit(peek())) {
                                throw ParseError("invalid unicode escape", pos_);
                            }
                            hex += consume();
                        }
                        int codepoint = std::stoi(hex, nullptr, 16);
                        if(codepoint < 0x80) {
                            str += static_cast<char>(codepoint);
                        } else if (codepoint < 0x800) {
                            str += static_cast<char>(0xC0 | (codepoint >> 6));
                            str += static_cast<char>(0x80 | (codepoint & 0x3F));
                        } else {
                            str += static_cast<char>(0xE0 | (codepoint >> 12));
                            str += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                            str += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        break;
                    }
                    default:
                        throw ParseError("invalid escape sequence", pos_-1);
                }
            } else {
                str += consume();
            }
        }
        expect('"');
        return JsonValue(std::move(str));

    }

    JsonValue parse_array() {
        expect('[');
        skip_whitespace();
        JsonArray arr;

        if(peek() == ']') {
            ++pos_;
            return JsonValue(std::move(arr));
        }

        while(true) {
            arr.push_back(parse_value());
            skip_whitespace();
            if(peek() == ']') {
                ++pos_;
                return JsonValue(std::move(arr));
            }
            expect(',');
            skip_whitespace();
        }
    }

    JsonValue parse_object() {
        expect('{');
        skip_whitespace();
        JsonObject obj;
        if(peek() == '}') {
            ++pos_;
            return JsonValue(std::move(obj));
        }
        while(true) {
            skip_whitespace();
            if(peek() != '"') {
                throw ParseError("expected string key", pos_);
            }
            auto key = parse_string().as_string();
            skip_whitespace();
            expect(':');
            auto value = parse_value();
            obj[std::move(key)] = std::move(value);
            skip_whitespace();
            if(peek() == '}') {
                ++pos_;
                return JsonValue(std::move(obj));
            }
            expect(',');
        }
    }
};

JsonValue parse(const std::string& json){
    Parser parser(json);
    return parser.parse();
}

}


