#include <gtest/gtest.h>
#include "json_parser/json.hpp"

// parsing tests -----------------------------------------------------------------
TEST(JsonParse, Null){
    auto val = json::parse("null");
    EXPECT_TRUE(val.is_null());
}
TEST(JsonParse, BoolTrue){
    auto val = json::parse("true");
    EXPECT_TRUE(val.is_bool());
    EXPECT_EQ(val.as_bool(), true);
}
TEST(JsonParse, BoolFalse){
    auto val = json::parse("false");
    EXPECT_TRUE(val.is_bool());
    EXPECT_EQ(val.as_bool(), false);
}

// note: use EXPECT_NEAR for comparing decimals with tolerance
TEST(JsonParse, Integer){
    auto val = json::parse("42");
    EXPECT_TRUE(val.is_number());
    EXPECT_EQ(val.as_number(), 42.0);
}
TEST(JsonParse, NegInteger){
    auto val = json::parse("-17");
    EXPECT_TRUE(val.is_number());
    EXPECT_EQ(val.as_number(), -17.0);
}
TEST(JsonParse, Decimal){
    auto val = json::parse("3.14");
    EXPECT_TRUE(val.is_number());
    EXPECT_NEAR(val.as_number(), 3.14, 0.001);
}
TEST(JsonParse, Exponent){
    auto val = json::parse("1e10");
    EXPECT_TRUE(val.is_number());
    EXPECT_EQ(val.as_number(), 1e10);
}
TEST(JsonParse, NegExponent){
    auto val = json::parse("5e-3");
    EXPECT_TRUE(val.is_number());
    EXPECT_NEAR(val.as_number(), 0.005, 0.0001);
}
TEST(JsonParse, Zero){
    auto val = json::parse("0");
    EXPECT_TRUE(val.is_number());
    EXPECT_EQ(val.as_number(), 0.0);
}
TEST(JsonParse, ZeroDecimal){
    auto val = json::parse("0.5");
    EXPECT_TRUE(val.is_number());
    EXPECT_NEAR(val.as_number(), 0.5, 0.001);
}

TEST(JsonParse, SimpleString){
    auto val = json::parse("\"hello\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "hello");
}
TEST(JsonParse, EmptyString){
    auto val = json::parse("\"\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "");
}
TEST(JsonParse, StringWithEscapes){
    auto val = json::parse("\"hello\\nworld\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "hello\nworld");
}
TEST(JsonParse, StringWithQuote){
    auto val = json::parse("\"say\\\"hi\\\"\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "say\"hi\"");
}
TEST(JsonParse, StringWithBackslash){
    auto val = json::parse("\"path\\\\to\\\\file\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "path\\to\\file");
}
TEST(JsonParse, StringWithUnicode){
    auto val = json::parse("\"\\u0041\"");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "A");
}

TEST(JsonParse, EmptyArray){
    auto val = json::parse("[]");
    EXPECT_TRUE(val.is_array());
    EXPECT_EQ(val.size(), 0);
}
TEST(JsonParse, SimpleArray){
    auto val = json::parse("[1, 2, 3]");
    EXPECT_TRUE(val.is_array());
    EXPECT_EQ(val.size(), 3);
    EXPECT_EQ(val[0].as_number(), 1.0);
    EXPECT_EQ(val[1].as_number(), 2.0);
    EXPECT_EQ(val[2].as_number(), 3.0);
}
TEST(JsonParse, MixedArray){
    auto val = json::parse("[1, \"two\", true, null]");
    EXPECT_TRUE(val.is_array());
    EXPECT_EQ(val.size(), 4);
    EXPECT_TRUE(val[0].is_number());
    EXPECT_TRUE(val[1].is_string());
    EXPECT_TRUE(val[2].is_bool());
    EXPECT_TRUE(val[3].is_null());

}
TEST(JsonParse, NestedArray){
    auto val = json::parse("[[1,2],[3,4]]");
    EXPECT_EQ(val.size(), 2);
    EXPECT_EQ(val[0][0].as_number(), 1.0);
    EXPECT_EQ(val[1][1].as_number(). 4.0);
}

TEST(JsonParse, EmptyObject){
    auto val = json::parse("{}");
    EXPECT_TRUE(val.is_object());
    EXPECT_EQ(val.size(), 0);
}
TEST(JsonParse, SimpleObject){
    auto val = json::parse("{\"name\"}: \"Alice\", \"age\": 30");
    EXPECT_TRUE(val.is_object());
    EXPECT_EQ(val["name"].as_string(), "Alice");
    EXPECT_EQ(val["age"].as_number(), 30.0);
}
TEST(JsonParse, NestedObject){
    auto val = json::parse("{\"persion\": {\"name\": \"Bob\"}}");
    EXPECT_EQ(val["person"]["name"].as_string(), "Bob");
}
TEST(JsonParse, ObjectWithArray){
    auto val = json::parse("{\"nums\": [1,2,3]}");
    EXPECT_EQ(val["nums"].size(), 3);
    EXPECT_EQ(val["nums"][0].as_number(), 1.0);
}
TEST(JsonParse, Whiespace){
    auto val = json::parse("    {     \"a\"    :  1   }    ");
    EXPECT_EQ(val["a"].as_number, 1.0);
}

// parse error tests -------------------------------------------------------------
TEST(JsonParse, InvalidLeadingZero){
    EXPECT_THROW(json::parse("007", json::ParseError));
}
TEST(JsonParse, InvalidLeadingPlus){
    EXPECT_THROW(json::parse("+5", json::ParseError));
}
TEST(JsonParse, InvalidDecimalNoLeadingDigit){
    EXPECT_THROW(json::parse(".5", json::ParseError));
}

TEST(JsonParse, InvalidTrailingCommaArray){
    EXPECT_THROW(json::parse("[1,2,]", json::ParseError));
}
TEST(JsonParse, InvalidTrailingCommaObject){
    EXPECT_THROW(json::parse("{\"a\":1,}", json::ParseError));
}

TEST(JsonParse, UnterminatedString){
    EXPECT_THROW(json::parse("\"hello", json::ParseError));
}
TEST(JsonParse, UnterminatedArray){
    EXPECT_THROW(json::parse("[1,2]", json::ParseError));
}
TEST(JsonParse, UnterminatedObject){
    EXPECT_THROW(json::parse("{\"a\": 1", json::ParseError));
}

TEST(JsonParse, InvalidToken){
    EXPECT_THROW(json::parse("undefined", json::ParseError));
}
TEST(JsonParse, TrailingGarbage){
    EXPECT_THROW(json::parse("123abc", json::ParseError));
}

// type access tets --------------------------------------------------------------
TEST(JsonParse, WrongTypeThrows){
    auto val = json::parse("42");
    EXPECT_THROW(val.as_string(), std::runtime_error);
    EXPECT_THROW(val.as_bool(), std::runtime_error);
    EXPECT_THROW(val.as_array(), std::runtime_error);
    EXPECT_THROW(val.as_object(), std::runtime_error);
}
TEST(JsonParse, ArrayOutOfBounds){
    auto val = json::parse("[1,2]");
    EXPECT_THROW(val[5], std::out_of_range);
}
TEST(JsonParse, ObjectMissingKey){
    auto val = json::parse("{\"a\":1}");
    const auto& cval = val;
    EXPECT_THROW(cval["missing"], std::out_of_range);
}

// dump tests --------------------------------------------------------------------
TEST(JsonParse, Null){
    json::JsonValue val(nullptr);
    EXPECT_EQ(val.dump(), "null");
}
TEST(JsonParse, Bool){
    EXPECT_EQ(json::JsonValue(true).dump(), "true");
    EXPECT_EQ(json::JsonValue(false).dump(), "false");
}
TEST(JsonParse, Number){
    json::JsonValue val(42.0);
    EXPECT_EQ(val.dump(), "42");
}
TEST(JsonParse, String){
    json::JsonValue val("hello");
    EXPECT_EQ(val.dump(), "\"hello\"");
}
TEST(JsonParse, StringWithEscapes){
    json::JsonValue val("line1\nline2");
    EXPECT_EQ(val.dump(), "\"line1\\nline2\"");
}
TEST(JsonParse, EmptyArray){
    json::JsonValue val(json::JsonArray{});
    EXPECT_EQ(val.dump(), "[]");
}
TEST(JsonParse, EmptyObject){
    json::JsonValue val(json::JsonObject{});
    EXPECT_EQ(val.dump(), "{}");
}
TEST(JsonParse, RoundTrip){
    std::string original = "{\"name\":\"Alice\",\"age\":30,\"active\":true}";
    auto val = json::parse(original);
    auto dumped = val.dump();
    auto reparsed = json::parse(dumped);
    EXPECT_EQ(reparsed["name".as_string(), "Alice"]);
    EXPECT_EQ(reparsed["age".as_number(), 30]);
    EXPECT_EQ(reparsed["active".as_bool(), true]);
}

// construction tests ------------------------------------------------------------
TEST(JsonParse, ConstructFromInt){
    json::JsonValue val(42);
    EXPECT_TRUE(val.is_number());
    EXPECT_EQ(val.as_number(), 42.0);
}
TEST(JsonParse, ConstructFromString){
    json::Jsonvalue val("hello");
    EXPECT_TRUE(val.is_string());
    EXPECT_EQ(val.as_string(), "hello");
}
TEST(JsonParse, MutableArrayAccess){
    json::JsonValue val(json::JsonArray{1,2,3});
    val[0] = 10;
    EXPECT_EQ(val[0].as_number(), 10.0);
}
TEST(JsonParse, MutableObjectAccess){
    json::JsonValue val(json::JsonObject{{"a" : 1}});
    val["b"] = 2;
    EXPECT_EQ(val["b"].as_number(), 2.0);
}

