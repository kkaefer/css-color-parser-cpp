#include "csscolorparser.hpp"

#include <iostream>

using namespace CSSColorParser;

std::ostream& operator<<(std::ostream& os, const optional<Color>& color) {
    if (color) {
        return os << "rgba(" << int(color->r) << ", " << int(color->g) << ", " << int(color->b)
                  << ", " << color->a << ")";
    } else {
        return os << "<no color>";
    }
}

static bool errored = false;

void ASSERT_EQUAL(const optional<Color>& expected, const std::string& input) {
    const auto actual = parse(input);
    if (expected != actual) {
        errored = true;
        std::cerr << "\033[1mERROR!: expected " << expected << " != parsed " << actual
                  << " when parsing \"" << input << "\"\033[0m" << std::endl;
    } else {
        std::cerr << "Passed: " << actual << " expected when parsing \"" << input << "\"" << std::endl;
    }
}

void ASSERT_EQUAL(const optional<Color>& expected, const optional<Color>& actual) {
    if (expected != actual) {
        errored = true;
        std::cerr << "\033[1mERROR!: expected " << expected << " != actual " << actual << "\"\033[0m" << std::endl;
    } else {
        std::cerr << "Passed: " << actual << " expected" << std::endl;
    }
}

int main() {
    try {
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, " rgba (255, 128, 12, 0.5)");
        ASSERT_EQUAL(Color{ 255, 255, 255, 1 }, "#fff");
        ASSERT_EQUAL(Color{ 255, 0, 17, 1 }, "#ff0011");
        ASSERT_EQUAL(Color{ 106, 90, 205, 1 }, "slateblue");
        ASSERT_EQUAL({}, "blah");
        ASSERT_EQUAL({}, "ffffff");
        ASSERT_EQUAL(Color{ 226, 233, 233, 0.5 }, "hsla(900, 15%, 90%, 0.5)");
        ASSERT_EQUAL({}, "hsla(900, 15%, 90%)");
        ASSERT_EQUAL(Color{ 226, 233, 233, 1 }, "hsl(900, 15%, 90%)");
        ASSERT_EQUAL(Color{ 226, 233, 233, 1 }, "hsl(900, 0.15, 90%)"); // NOTE: not spec compliamt.
        ASSERT_EQUAL(Color{ 0, 0, 0, 1 }, "hsl(9999999999999999999, 0, 0)"); // NOTE: float precision loss
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(45, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-315, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-675, 100%, 50%)");

        // Out of range:
        ASSERT_EQUAL({}, "xxx");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, " rgba (255, 128, 12, 2)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, " rgba (400, 128, 12, 2)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, Color{ 255, 128, 12, 3 });
    } catch(std::exception& ex) {
        std::cerr << "EXCEPTION!: " << ex.what() << std::endl;
        return 2;
    }

    return errored ? 1 : 0;
}
