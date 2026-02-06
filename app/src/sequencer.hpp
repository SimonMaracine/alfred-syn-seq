#pragma once

#include <vector>
#include <unordered_map>
#include <flat_set>
#include <set>
#include <stdexcept>
#include <functional>

#include <alfred/synthesizer.hpp>

namespace seq {
    using Beats = unsigned int;

    enum Value : unsigned int {
        Whole = 1,
        Half = 2,
        Quarter = 4,
        Eighth = 8,
        Sixteenth = 16
    };

    // A sixteenth divided by three is the step size
    inline constexpr unsigned int DIV {3};

    // A step has variable length in seconds depending on the time signature and tempo
    inline constexpr unsigned int STEP {Sixteenth * DIV};

    // Quarters per minute
    class Tempo {
    public:
        static constexpr unsigned int MIN {4};
        static constexpr unsigned int MAX {240};

        constexpr Tempo() = default;
        constexpr explicit Tempo(unsigned int tempo)
            : m_tempo(tempo) {}

        constexpr operator unsigned int() const { return m_tempo; }
    private:
        unsigned int m_tempo {90};
    };

    class TimeSignature {
    public:
        constexpr TimeSignature() = default;
        constexpr TimeSignature(Beats beats, Value value)
            : m_beats(beats), m_value(value) {}

        constexpr Beats beats() const { return m_beats; }
        constexpr Value value() const { return m_value; }

        constexpr unsigned int measure_steps() const {
            return m_beats * (STEP / m_value);
        }

        constexpr double step_time(Tempo tempo) const {
            return 1.0 / (steps_per_minute(tempo) / 60.0);
        }
    private:
        // Transforms tempo quarters per minute into beats per minute
        constexpr double steps_per_minute(Tempo tempo) const {
            return double(tempo) / (double(Quarter) / double(m_value)) * double(STEP / m_value);
        }

        Beats m_beats {4};
        Value m_value {Quarter};
    };

    struct Note {
        syn::Id id {};
        Value value {};
        unsigned int position {};  // Local, inside a measure
        bool legato {};

        // Order the notes in the measure bottom to top, left to right
        bool operator<(const Note& other) const {
            if (id < other.id) {
                return true;
            }

            if (id == other.id) {
                return position < other.position;
            }

            return false;
        }
    };

    struct Measure {
        Tempo tempo;
        TimeSignature time_signature;

        // Notes must always be sorted in a very specific way
        // Use a normal set, because the iterators need to stay stable
        std::unordered_map<syn::Voice, std::multiset<Note>> voices;  // TODO erase empty voices when serializing
    };

    struct Composition {
        std::vector<Measure> measures;

        void validate() const;
    };

    namespace exec {
        struct Note {
            Note(syn::Id id, unsigned int position, unsigned int duration, Tempo tempo, TimeSignature time_signature)
                : id(id), position(position), duration(duration), tempo(tempo), time_signature(time_signature) {}

            syn::Id id {};

            unsigned int position {};  // Global, in the whole composition
            unsigned int duration {};  // Number of steps

            // Needed to know how fast to play the note
            Tempo tempo;
            TimeSignature time_signature;
        };

        struct UnplayedNote : Note {
            using Note::Note;

            bool operator<(const UnplayedNote& other) const {
                return position < other.position;
            }
        };

        struct PlayedNote : Note {
            using Note::Note;

            bool operator<(const PlayedNote& other) const {
                return position + duration < other.position + other.duration;
            }
        };

        // Notes must always be ordered
        using UnplayedNotes = std::flat_multiset<UnplayedNote>;
        using PlayedNotes = std::flat_multiset<PlayedNote>;

        struct Execution {
            UnplayedNotes notes_unplayed;
            PlayedNotes notes_played;
        };

        using Executions = std::unordered_map<syn::Voice, Execution>;
    }

    class Player {
    public:
        Player() = default;
        Player(synthesizer::Synthesizer& synthesizer, const Composition& composition, std::function<void()> stopped);

        void prepare();
        void start();
        void stop();
        void seek(unsigned int position);

        double get_elapsed_time() const { return m_elapsed_time; }
        unsigned int get_position() const { return m_position; }
        bool is_playing() const { return m_playing; }
        bool is_in_time() const { return m_in_time; }

        void update(double dt);
    private:
        using MeasureIter = std::vector<Measure>::const_iterator;

        void initialize(unsigned int position);
        exec::Executions initialize_executions(unsigned int position) const;
        MeasureIter initialize_measure(unsigned int position) const;
        double initialize_elapsed_time(unsigned int position) const;
        unsigned int initialize_measure_position(unsigned int position) const;
        bool finished() const;
        bool no_notes() const;

        synthesizer::Synthesizer* m_synthesizer {};
        const Composition* m_composition {};
        std::function<void()> m_stopped;

        exec::Executions m_executions;
        MeasureIter m_measure;
        double m_accumulator_time {};
        double m_elapsed_time {};
        unsigned int m_position {};  // Like a cursor
        unsigned int m_measure_position {};
        bool m_playing {};
        bool m_in_time {true};  // Is the player able to keep up with the piece
    };

    struct SequencerError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
