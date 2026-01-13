#pragma once

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

    enum Beats : unsigned int {
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
        Value2,
        Value4,
        Value8,
        Value16
    };

    struct TimeSignature {
        Beats beats {Beats4};
        Value value {Value4};
    };

    using Tempo = unsigned int;

    float rem(float size);
    ImVec2 rem(ImVec2 size);
    void set_scale(int scale);
}
