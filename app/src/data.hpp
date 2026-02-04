#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <cereal/cereal.hpp>

#include "ui.hpp"
#include "utility.hpp"

namespace data {
    struct Data {
        ui::ColorScheme color_scheme {};
        ui::Scale scale {};
        std::vector<std::string> recent_compositions;

        template<typename Archive>
        void serialize(Archive& archive, const std::uint32_t) {
            archive(color_scheme, scale, recent_compositions);
        }
    };

    inline constexpr std::uint32_t VERSION {1};

    void export_data(const Data& data, utility::Buffer& buffer);
    void import_data(Data& data, const utility::Buffer& buffer);

    std::filesystem::path file_path();

    struct DataError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };;
}

CEREAL_CLASS_VERSION(data::Data, data::VERSION)
