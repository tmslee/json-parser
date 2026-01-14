#include "json_parser/json.hpp"
#include <iostream>

int main() {
    std::string input = R"({
        "name": "Alice",
        "age": 30,
        "active": true,
        "scores": [95, 87, 92],
        "address": {
            "city": "Boston",
            "zip": "02101"
        }
    })";
    auto val = json::parse(input);

    //access values
    std::cout << "Name: " << val["name"].as_string() << "\n";
    std::cout << "Age: " << val["age"].as_number() << "\n";
    std::cout << "Active: " << (val["active"].as_bool() ? "yes" : "no") << "\n";

    //access nested array
    std::cout << "Scores: ";
    for(std::size_t i=0; i<val["scores"].size(); ++i) {
        std::cout << val["scores"][i].as_number() << " ";
    }
    std::cout << "\n";

    //access nested object
    std::cout << "City: " << val["address"]["city"].as_string() << "\n";

    //modify and dump
    val["age"] = 31;
    std::cout << "\nModified JSON:\n" << val.dump(2) << "\n";

    //create JSON programmatically
    json::JsonValue obj(json::JsonObject {
        {"id", 123},
        {"tags", json::JsonArray{"cpp", "json", "parser"}}
    });

    std::cout << "\nCreated JSON:\n" << obj.dump(2) << "\n";

    return 0;
}