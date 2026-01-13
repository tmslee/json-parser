#include <gtest/gtest.h>
#include "json_parser/json.hpp"

// parsing tests -----------------------------------------------------------------
TEST(JsonParse, Null){}
TEST(JsonParse, BoolTrue){}
TEST(JsonParse, BoolFalse){}

TEST(JsonParse, Integer){}
TEST(JsonParse, NegInteger){}
TEST(JsonParse, Decimal){}
TEST(JsonParse, Exponent){}
TEST(JsonParse, NegExponent){}
TEST(JsonParse, Zero){}
TEST(JsonParse, ZeroDecimal){}

TEST(JsonParse, SimpleString){}
TEST(JsonParse, EmptyString){}
TEST(JsonParse, StringWithEscapes){}
TEST(JsonParse, StringWithQuote){}
TEST(JsonParse, StringWithBackslash){}
TEST(JsonParse, StringWithUnicode){}

TEST(JsonParse, EmptyArray){}
TEST(JsonParse, SimpleArray){}
TEST(JsonParse, MixedArray){}
TEST(JsonParse, NestedArray){}

TEST(JsonParse, EmptyObject){}
TEST(JsonParse, SimpleObject){}
TEST(JsonParse, NestedObject){}
TEST(JsonParse, ObjectWithArray){}
TEST(JsonParse, Whiespace){}

// parse error tests -------------------------------------------------------------
TEST(JsonParse, InvalidLeadingZero){}
TEST(JsonParse, InvalidLeadingPlus){}
TEST(JsonParse, InvalidDecimalNoLeadingDigit){}

TEST(JsonParse, InvalidTrailingCommaArray){}
TEST(JsonParse, InvalidTrailingCommaObject){}

TEST(JsonParse, UnterminatedString){}
TEST(JsonParse, UnterminatedArray){}
TEST(JsonParse, UnterminatedObject){}

TEST(JsonParse, InvalidToken){}
TEST(JsonParse, TrailingGarbage){}

// type access tets --------------------------------------------------------------
TEST(JsonParse, WrongTypeThrows){}
TEST(JsonParse, ArrayOutOfBounds){}
TEST(JsonParse, ObjectMissingKey){}

// dump tests --------------------------------------------------------------------
TEST(JsonParse, Null){}
TEST(JsonParse, Bool){}
TEST(JsonParse, Number){}
TEST(JsonParse, String){}
TEST(JsonParse, StringWithEscapes){}
TEST(JsonParse, EmptyArray){}
TEST(JsonParse, EmptyObject){}
TEST(JsonParse, RoundTrip){}

// construction tests ------------------------------------------------------------
TEST(JsonParse, ConstructFromInt){}
TEST(JsonParse, ConstructFromString){}
TEST(JsonParse, MutableArrayAccess){}
TEST(JsonParse, MutableObjectAccess){}

