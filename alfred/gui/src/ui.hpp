#pragma once

namespace ui {
    enum ColorScheme {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
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
}
