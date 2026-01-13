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

    //access nested array

    //access nested object

    //modify and dump

    //create JSON programmatically
    
    return 0;
}