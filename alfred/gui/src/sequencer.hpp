#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <stdexcept>

#include <alfred/synthesizer.hpp>

enum Value : unsigned int {
    Whole = 1,
    Half = 2,
    Quarter = 4,
    Eighth = 8,
    Sixteenth = 16
};

class Tempo {
public:
    static constexpr unsigned int MIN {1};
    static constexpr unsigned int MAX {270};

    operator unsigned int() const { return m_tempo; }
private:
    unsigned int m_tempo {90};
};

class TimeSignature {
public:
    constexpr TimeSignature() = default;
    constexpr TimeSignature(unsigned int beats, Value value)
        : m_beats(beats), m_value(value) {}

    constexpr unsigned int beats() const { return m_beats; }
    constexpr Value value() const { return m_value; }

    constexpr unsigned int measure_steps() const {
        return m_beats * (Sixteenth / m_value);
    }

    constexpr unsigned int steps_per_minute(Tempo tempo) const {
        return tempo * (Sixteenth / m_value);
    }

    constexpr double step_time(Tempo tempo) const {
        return 1.0 / (double(steps_per_minute(tempo)) / 60.0);
    }
private:
    unsigned int m_beats {4};
    Value m_value {Quarter};
};

struct Note {
    syn::Name name {};
    syn::Octave octave {};
    Value value {};
    unsigned int position {};  // Compositions are divided in sixteenth steps
};

struct Measure {
    TimeSignature time_signature;
    Tempo tempo;
};

struct Composition {
    std::string title;
    std::string author;
    std::vector<Measure> measures;
    std::unordered_map<syn::Voice, std::vector<Note>> voices;

    void validate() const;
};

class Player {
public:
    Player() = default;
    Player(synthesizer::Synthesizer& synthesizer, const Composition& composition);

    void start();
    void stop();
    void seek(unsigned int position);
    void reload();
    double get_elapsed_time() const { return m_elapsed_time; }
    unsigned int get_position() const { return m_position; }
    bool is_playing() const { return m_playing; }

    void update(double dt);
private:
    struct Execution {
        struct Time {
            double begin {};
            double end {};
        };

        using Notes = std::vector<std::pair<Note, Time>>;

        Notes notes_unplayed;
        Notes notes_played;
    };

    using Executions = std::unordered_map<syn::Voice, Execution>;

    void initialize(unsigned int position);
    Executions initialize_executions(unsigned int position) const;
    Execution::Notes::value_type initialize_note(const Note& note) const;
    double initialize_time(unsigned int position) const;
    bool done() const;

    synthesizer::Synthesizer* m_synthesizer {};
    const Composition* m_composition {};
    Executions m_executions;
    double m_elapsed_time {};
    double m_accumulator_time {};
    unsigned int m_position {};  // Like a cursor
    bool m_playing {};
};

struct SequencerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
