#include "ui.hpp"

#include <cmath>
#include <cstring>

namespace alfred::ui {
    static void set_font(float scale) {
        const float font_size = std::floor(FONT_SIZE * scale);

        ImGuiIO& io = ImGui::GetIO();

        io.Fonts->Clear();

        ImFontConfig config;
        config.SizePixels = font_size;

        const auto font = io.Fonts->AddFontDefaultVector(&config);

        if (!font) {
            throw UiError("Could not add font");
        }

        io.FontDefault = font;
    }

    static void reset_style() {
        const ImGuiStyle current_style = ImGui::GetStyle();

        ImGuiStyle& style = ImGui::GetStyle();

        style = ImGuiStyle();

        std::memcpy(ImGui::GetStyle().Colors, current_style.Colors, sizeof(current_style.Colors));

        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
        style.TabBarBorderSize = 0.0f;
        style.WindowRounding = 4.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.WindowMenuButtonPosition = ImGuiDir_None;
    }

    float rem(float size) {
        return ImGui::GetFontSize() * size;
    }

    ImVec2 rem(ImVec2 size) {
        return { ImGui::GetFontSize() * size.x, ImGui::GetFontSize() * size.y };
    }

    void set_style() {
        reset_style();
    }

    void set_scale(float scale) {
        set_font(scale);
        reset_style();
        ImGui::GetStyle().ScaleAllSizes(scale);
    }

    void set_color_scheme(ColorScheme color_scheme) {
        switch (color_scheme) {
            case ColorSchemeDark:
                ImGui::StyleColorsDark();
                break;
            case ColorSchemeLight:
                ImGui::StyleColorsLight();
                break;
            case ColorSchemeClassic:
                ImGui::StyleColorsClassic();
                break;
        }
    }
}
