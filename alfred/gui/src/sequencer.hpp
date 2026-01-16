#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <flat_set>
#include <utility>
#include <stdexcept>
#include <chrono>

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

    class Tempo {
    public:
        static constexpr unsigned int MIN {1};
        static constexpr unsigned int MAX {240};

        constexpr Tempo() = default;
        constexpr Tempo(unsigned int tempo)
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

        constexpr unsigned int steps_per_minute(Tempo tempo) const {
            return tempo * (STEP / m_value);
        }

        constexpr double step_time(Tempo tempo) const {
            return 1.0 / (double(steps_per_minute(tempo)) / 60.0);
        }
    private:
        Beats m_beats {4};
        Value m_value {Quarter};
    };

    struct Note {
        syn::Name name {};
        syn::Octave octave {};
        Value value {};
        unsigned int position {};  // Local, inside a measure

        bool operator<(const Note& other) const {
            return position < other.position;
        }
    };

    struct Measure {
        Tempo tempo;
        TimeSignature time_signature;
        std::unordered_map<syn::Voice, std::flat_multiset<Note>> voices;  // Notes must always be sorted
    };

    struct Composition {
        std::string title;
        std::string author;
        std::chrono::year year;
        std::vector<Measure> measures;

        void validate() const;
    };

    namespace exec {
        struct Note {
            Note(syn::Name name, syn::Octave octave, unsigned int position, unsigned int duration, Tempo tempo, TimeSignature time_signature)
                : name(name), octave(octave), position(position), duration(duration), tempo(tempo), time_signature(time_signature) {}

            syn::Name name {};
            syn::Octave octave {};

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
        Player(synthesizer::Synthesizer& synthesizer, const Composition& composition);

        void prepare();
        void start();
        void stop();
        void seek(unsigned int position);

        double get_elapsed_time() const { return m_elapsed_time; }
        unsigned int get_position() const { return m_position; }
        bool is_playing() const { return m_playing; }

        void update(double dt);  // FIXME when stopped, the application accumulates time and then goes crazy
    private:
        void initialize(unsigned int position);
        exec::Executions initialize_executions(unsigned int position) const;
        unsigned int initialize_measure_position(unsigned int position) const;
        double initialize_time(unsigned int position) const;
        bool finished() const;
        bool no_notes() const;

        synthesizer::Synthesizer* m_synthesizer {};
        const Composition* m_composition {};

        exec::Executions m_executions;

        std::vector<Measure>::const_iterator m_measure;
        double m_accumulator_time {};
        double m_elapsed_time {};
        unsigned int m_position {};  // Like a cursor
        unsigned int m_measure_position {};
        bool m_playing {};
    };

    struct SequencerError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
