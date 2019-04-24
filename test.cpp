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
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, " rgb (255, 128, 12)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, "rgb(255,128,12)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, "RGB(255,128,12)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, "rgb( 255 , 128 , 12 )");
        ASSERT_EQUAL(Color{ 255, 127, 0, 1 }, "rgb(100%, 50%, 0%)");
        ASSERT_EQUAL(Color{ 255, 127, 0, 0.5 }, "rgb(100%, 50%, 0%, 50%)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "rgba(255, 128, 12, 0.5)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "RGBa(255, 128, 12, 0.5)");
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "rgba(255, 128, 12, 50%)");
        ASSERT_EQUAL(Color{ 255, 255, 255, 1 }, "#fff");
        ASSERT_EQUAL(Color{ 255, 255, 255, 1 }, "#FFF");
        ASSERT_EQUAL(Color{ 255, 255, 255, 1 }, "#fFf");
        ASSERT_EQUAL(Color{ 255, 0, 17, 1 }, "#ff0011");
        ASSERT_EQUAL(Color{ 255, 255, 255, 1 }, "#ffff"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 255, 255, 0 }, "#fff0"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 0, 17, 1 }, "#ff0011ff"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 0, 17, 0 }, "#ff001100"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 128, 12, 1 }, "rgb(255 128 12)"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "rgb(255, 128, 12, 0.5)"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "rgb(255 128 12 / 0.5)"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 128, 12, 0.5 }, "rgba(255 128 12 / 0.5)"); // NOTE: level 4
        ASSERT_EQUAL({}, "rgb(255 128 12 0.5)"); // NOTE: slash is required before alpha
        ASSERT_EQUAL(Color{ 255, 127, 0, 1 }, "rgb(100% 50% 0%)"); // NOTE: level 4
        ASSERT_EQUAL(Color{ 255, 127, 0, 0.5 }, "rgb(100% 50% 0% / 50%)"); // NOTE: level 4
        ASSERT_EQUAL({}, "rgb(100% 50% 0% 50%)"); // NOTE: slash is required before alpha
        ASSERT_EQUAL(Color{ 0, 0, 255, 1 }, "blue");
        ASSERT_EQUAL(Color{ 0, 0, 255, 1 }, "Blue");
        ASSERT_EQUAL(Color{ 0, 0, 255, 1 }, "BLUE");
        ASSERT_EQUAL(Color{ 112, 128, 144, 1 }, "slategray");
        ASSERT_EQUAL(Color{ 112, 128, 144, 1 }, "slategrey");
        ASSERT_EQUAL(Color{ 112, 128, 144, 1 }, "SlateGrey");
        ASSERT_EQUAL({}, "slate-grey");
        ASSERT_EQUAL(Color{ 0, 0, 0, 0 }, "transparent"); // NOTE: transparent black
        ASSERT_EQUAL({}, "blah"); // NOTE: invalid color name
        ASSERT_EQUAL({}, "#fffff"); // NOTE: invalid hex code, five digits
        ASSERT_EQUAL({}, "ffffff"); // NOTE: invalid hex code, no hash prefix
        ASSERT_EQUAL(Color{ 226, 233, 233, 0.5 }, "hsla(900, 15%, 90%, 0.5)");
        ASSERT_EQUAL(Color{ 226, 233, 233, 0.5 }, "hsla(900, 15%, 90%, 50%)");
        ASSERT_EQUAL({}, "hsla(900, 15%, 90%)"); // NOTE: missing alpha value
        ASSERT_EQUAL(Color{ 226, 233, 233, 1 }, "hsl(900, 15%, 90%)");
        ASSERT_EQUAL(Color{ 226, 233, 233, 1 }, "hsl(900, 0.15, 90%)"); // NOTE: not spec compliant
        ASSERT_EQUAL(Color{ 0, 0, 0, 1 }, "hsl(9999999999999999999, 0, 0)"); // NOTE: float precision loss
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(45, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-315, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-675, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(45, 100%, 50%)"); // NOTE: degree unit is implied
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(45deg, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(45DEG, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(405deg, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-315deg, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(-675deg, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(0.785398rad, 100%, 50%)"); // NOTE: pi/4
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(0.125turn, 100%, 50%)");
        ASSERT_EQUAL(Color{ 255, 191, 0, 1 }, "hsl(50grad, 100%, 50%)");

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
