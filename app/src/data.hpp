#pragma once

#include <set>
#include <string>

#include <cereal/cereal.hpp>

#include "ui.hpp"
#include "utility.hpp"

namespace data {
    struct Data {
        bool show_keyboard {true};
        ui::ColorScheme color_scheme {ui::ColorSchemeDark};
        ui::Scale scale {ui::Scale100};
        std::set<std::string> recent_compositions;

        template<typename Archive>
        void serialize(Archive& archive, const std::uint32_t) {
            archive(show_keyboard, color_scheme, scale, recent_compositions);
        }
    };

    void export_data(const Data& data, utility::Buffer& buffer);
    void import_data(Data& data, const utility::Buffer& buffer);

    struct DataError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

CEREAL_CLASS_VERSION(data::Data, 1)
