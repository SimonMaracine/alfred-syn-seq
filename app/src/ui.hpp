#pragma once

#include <utility>

#include <imgui.h>

namespace ui {
    inline constexpr float FONT_SIZE {13.0f};

    enum ColorScheme : unsigned int {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
    };

    enum Scale : unsigned int {
        Scale1X,
        Scale2X
    };

    enum Tool : int {
        ToolMeasure,
        ToolNote
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

    using Tempo = unsigned int;

    enum Value : int {
        ValueWhole,
        ValueHalf,
        ValueQuarter,
        ValueEighth,
        ValueSixteenth
    };

    enum Voice : unsigned int {
        VoiceBell,
        VoiceHarmonica,
        VoiceDrumKick
    };

    enum Octave : int {
        Octave1 = 1,
        Octave2,
        Octave3,
        Octave4,
        Octave5
    };

    // https://www.colorhexa.com/color-names

    using ColorIndex = unsigned int;

    inline constexpr std::pair<const char*, ImColor> COLORS[] {
        { "American rose", ImColor(255, 3, 62) },
        { "Amethyst", ImColor(153, 102, 204) },
        { "Android green", ImColor(164, 198, 67) },
        { "Antique fuchsia", ImColor(145, 92, 131) },
        { "Ao", ImColor(0, 128, 0) },
        { "Amber", ImColor(255, 191, 0) },
        { "Azure", ImColor(0, 127, 255) },
        { "Bittersweet", ImColor(254, 111, 94) },
        { "Blue violet", ImColor(138, 43, 226) },
        { "Blue", ImColor(0, 0, 255) },
        { "Brightpink", ImColor(255, 0, 127) },
        { "Bronze", ImColor(205, 127, 50) }
    };

    float rem(float size);
    ImVec2 rem(ImVec2 size);
    void set_scale(int scale);
}
