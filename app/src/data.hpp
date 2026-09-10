#pragma once

#include <set>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>

#include "ui.hpp"
#include "utility.hpp"

// Application settings/options + serialization

namespace alfred::data {
    struct Data {
        bool show_keyboard = true;
        ui::ColorScheme color_scheme = ui::ColorSchemeDark;
        ui::Scale scale = ui::Scale100;
        std::set<std::string> recent_compositions;

        template<typename Archive>
        void serialize(Archive& archive, const std::uint32_t) {
            archive(show_keyboard, color_scheme, scale, recent_compositions);
        }
    };

    void export_data(const Data& data, utility::Buffer& buffer);
    void import_data(Data& data, const utility::Buffer& buffer);
}

CEREAL_CLASS_VERSION(alfred::data::Data, 1)
