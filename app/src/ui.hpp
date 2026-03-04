#pragma once

#include <utility>
#include <stdexcept>

#include <imgui.h>

namespace ui {
    inline constexpr float FONT_SIZE {13.0f};

    enum ColorScheme : unsigned int {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
    };

    enum Scale : unsigned int {
        Scale100,
        Scale125,
        Scale150,
        Scale175,
        Scale200
    };

    enum Tool : int {
        ToolMeasure,
        ToolNote
    };

    enum Loudness : int {
        LoudnessPianississimo,
        LoudnessPianissimo,
        LoudnessPiano,
        LoudnessMezzoPiano,
        LoudnessMezzoForte,
        LoudnessForte,
        LoudnessFortissimo,
        LoudnessFortississimo
    };

    struct Dynamics {
        bool varying {};
        Loudness loudness1 {LoudnessMezzoForte};
        Loudness loudness2 {LoudnessMezzoForte};
    };

    using Tempo = unsigned int;

    struct Agogic {
        bool varying {};
        Tempo tempo1 {90};
        Tempo tempo2 {90};
    };

    struct TimeSignature {
        enum Beats : unsigned int {
            Beats1,
            Beats2,
            Beats3,
            Beats4,
            Beats5,
            Beats6,
            Beats7,
            Beats8,
            Beats9,
            Beats10,
            Beats11,
            Beats12,
            Beats13,
            Beats14,
            Beats15,
            Beats16
        };

        enum Value : unsigned int {
            Value1,
            Value2,
            Value4,
            Value8,
            Value16
        };

        Beats beats {Beats4};
        Value value {Value4};
    };

    enum Value : int {
        ValueWhole,
        ValueHalf,
        ValueQuarter,
        ValueEighth,
        ValueSixteenth
    };

    enum Octave : int {
        OctaveFirst,
        OctaveSecond,
        OctaveThird,
        OctaveFourth,
        OctaveFifth,
        OctaveSixth,
        OctaveSeventh
    };

    // https://www.colorhexa.com/color-names

    using ColorIndex = unsigned int;

    inline constexpr std::pair<const char*, ImColor> COLORS[] {
        { "Ash grey", ImColor(178, 190, 181) },
        { "Debian red", ImColor(215, 10, 83) },
        { "Amethyst", ImColor(153, 102, 204) },
        { "Bright pink", ImColor(255, 0, 127) },
        { "India green", ImColor(19, 136, 8) },
        { "Azure", ImColor(0, 127, 255) },
        { "Lemon", ImColor(255, 247, 0) },
        { "Antique fuchsia", ImColor(145, 92, 131) },
        { "Pastel blue", ImColor(174, 198, 207) },
        { "Bittersweet", ImColor(254, 111, 94) },
        { "Pale lavender", ImColor(220, 208, 255) },
        { "Blue violet", ImColor(138, 43, 226) },
        { "Medium Blue", ImColor(0, 0, 205) },
        { "Android green", ImColor(164, 198, 67) },
        { "Bright green", ImColor(102, 255, 0) },
        { "Bronze", ImColor(205, 127, 50) },
        { "Army green", ImColor(75, 83, 32) },
        { "Amber", ImColor(255, 191, 0) }
    };

    struct Composition {
        char title[64] {};
        char author[64] {};
        short year {};
    };

    float rem(float size);
    ImVec2 rem(ImVec2 size);
    void set_style();
    void set_scale(float scale);
    void set_color_scheme(ColorScheme color_scheme);

    constexpr float scale(Scale scale) {
        switch (scale) {
            case Scale100:
                return 1.0f;
            case Scale125:
                return 1.25f;
            case Scale150:
                return 1.5f;
            case Scale175:
                return 1.75f;
            case Scale200:
                return 2.0f;
        }

        std::unreachable();
    }

    struct UiError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
