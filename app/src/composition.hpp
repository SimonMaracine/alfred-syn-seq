#pragma once

#include <string>
#include <chrono>
#include <stdexcept>

#include <cereal/cereal.hpp>
#include <cereal/types/base_class.hpp>

#include "sequencer.hpp"
#include "utility.hpp"

namespace seq {
    template<typename Archive>
    void save(Archive& archive, const Tempo& self, const std::uint32_t) {
        archive(static_cast<unsigned int>(self));
    }

    template<typename Archive>
    void load(Archive& archive, Tempo& self, const std::uint32_t) {
        unsigned int tempo {};
        archive(tempo);
        self = Tempo(tempo);
    }

    template<typename Archive>
    void save(Archive& archive, const TimeSignature& self, const std::uint32_t) {
        archive(self.beats(), self.value());
    }

    template<typename Archive>
    void load(Archive& archive, TimeSignature& self, const std::uint32_t) {
        Beats beats {};
        Value value {};
        archive(beats, value);
        self = TimeSignature(beats, value);
    }

    template<typename Archive>
    void serialize(Archive& archive, Note& self, const std::uint32_t) {
        archive(self.id, self.value, self.position, self.legato);
    }

    template<typename Archive>
    void serialize(Archive& archive, Measure& self, const std::uint32_t) {
        archive(self.tempo, self.time_signature, self.instruments);
    }

    template<typename Archive>
    void serialize(Archive& archive, Composition& self, const std::uint32_t) {
        archive(self.measures);
    }
}

namespace std::chrono {
    template<typename Archive>
    void save(Archive& archive, const year& self, const std::uint32_t) {
        archive(int(self));
    }

    template<typename Archive>
    void load(Archive& archive, year& self, const std::uint32_t) {
        int tempo {};
        archive(tempo);
        self = year(tempo);
    }
}

namespace composition {
    struct Composition : seq::Composition {
        std::string title;
        std::string author;
        std::chrono::year year {};

        template<typename Archive>
        void serialize(Archive& archive, const std::uint32_t) {
            archive(cereal::base_class<seq::Composition>(this), title, author, year);
        }
    };

    inline constexpr std::uint32_t VERSION {1};

    void export_composition(const Composition& composition, utility::Buffer& buffer);
    void import_composition(Composition& composition, const utility::Buffer& buffer);

    struct CompositionError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

CEREAL_CLASS_VERSION(composition::Composition, composition::VERSION)
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(composition::Composition, cereal::specialization::member_serialize)
