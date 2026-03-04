#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <optional>
#include <variant>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <iterator>

#include <alfred/synthesizer.hpp>
#include <alfred/math.hpp>

#include "flat_set.hpp"

namespace seq {
    using Beats = unsigned int;

    enum Value : unsigned int {
        Whole = 1,
        Half = 2,
        Quarter = 4,
        Eighth = 8,
        Sixteenth = 16
    };

    // A sixteenth divided by this is the step size
    // A step has variable length in seconds depending on the time signature and tempo
    inline constexpr unsigned int DIVISION {3 * 5};

    // Chosen semi-arbitrarily :P
    inline constexpr unsigned int MIN_DURATION {3};

    inline constexpr unsigned int DELAY_INCREMENT {1};
    inline constexpr unsigned int MAX_DELAY {6};

    constexpr unsigned int steps(Value value) {
        return Sixteenth * DIVISION / value;
    }

    // Quarters per minute
    class Tempo {
    public:
        static constexpr unsigned int MIN {30};
        static constexpr unsigned int MAX {240};

        constexpr Tempo() = default;
        constexpr explicit Tempo(unsigned int tempo)
            : m_tempo(tempo) {}

        constexpr operator unsigned int() const { return m_tempo; }
        constexpr auto operator<=>(const Tempo&) const = default;
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

        constexpr auto operator<=>(const TimeSignature&) const = default;

        constexpr unsigned int measure_steps() const {
            return m_beats * steps(m_value);
        }

        constexpr double step_time(Tempo tempo) const {
            return 1.0 / (steps_per_minute(tempo) / 60.0);
        }
    private:
        // Transforms tempo quarters per minute into beats per minute
        constexpr double steps_per_minute(Tempo tempo) const {
            return double(tempo) / (double(Quarter) / double(m_value)) * double(steps(m_value));
        }

        Beats m_beats {4};
        Value m_value {Quarter};
    };

    enum class Loudness {
        Pianississimo,
        Pianissimo,
        Piano,
        MezzoPiano,
        MezzoForte,
        Forte,
        Fortissimo,
        Fortississimo
    };

    constexpr double loudness_amplitude(Loudness loudness) {
        const double value {math::map(double(loudness), double(Loudness::Pianississimo), double(Loudness::Fortississimo), 0.2, 1.0)};
        return value * value;
    }

    struct Note {
        syn::NoteId id {};
        Value value {};
        unsigned int position {};  // Local, inside a measure
        unsigned int delay {};  // Used for arpeggios
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

    using Notes = std::set<Note>;

    struct ConstantLoudness {
        Loudness loudness {Loudness::MezzoForte};
    };

    struct VaryingLoudness {
        Loudness loudness_begin {Loudness::MezzoForte};
        Loudness loudness_end {Loudness::MezzoForte};
    };

    using Dynamics = std::variant<ConstantLoudness, VaryingLoudness>;

    struct ConstantTempo {
        Tempo tempo;
    };

    struct VaryingTempo {
        Tempo tempo_begin;
        Tempo tempo_end;
    };

    using Agogic = std::variant<ConstantTempo, VaryingTempo>;

    struct Measure {
        TimeSignature time_signature;
        Dynamics dynamics;
        Agogic agogic;

        // Notes must always be sorted in a very specific way
        // Use a normal set, because the iterators need to stay stable
        std::unordered_map<syn::InstrumentId, Notes> instruments;  // TODO erase empty when serializing

        bool equal_signature(const Measure& other) const {
            return time_signature == other.time_signature;
        }
    };

    using MeasureIter = std::vector<Measure>::iterator;
    using ConstMeasureIter = std::vector<Measure>::const_iterator;
    using NoteIter = Notes::iterator;

    template<std::contiguous_iterator MeasureIter_ = MeasureIter>
    class ProvenanceNote {
    public:
        ProvenanceNote() = default;
        ProvenanceNote(MeasureIter_ measure, NoteIter note)
            : m_measure(measure), m_note(note) {}

        MeasureIter_ measure() const { return m_measure; }
        NoteIter note() const { return m_note; }
        void note(NoteIter note) { m_note = note; }
        Note copy() const { return *m_note; }
    private:
        MeasureIter_ m_measure;
        NoteIter m_note;
    };

    struct Composition {
        std::vector<Measure> measures;

        // In steps
        unsigned int size() const;

        static bool note_first_in_measure(const Measure& measure, const Note& note);
        static bool note_last_in_measure(const Measure& measure, const Note& note);

        template<std::contiguous_iterator MeasureIter>
        std::optional<ProvenanceNote<MeasureIter>> check_note_has_next(syn::InstrumentId instrument, const ProvenanceNote<MeasureIter>& provenance_note) const {
            return check_note_has_next(instrument, provenance_note.measure(), provenance_note.note());
        }

        template<std::contiguous_iterator MeasureIter>
        std::optional<ProvenanceNote<MeasureIter>> check_note_has_next(syn::InstrumentId instrument, MeasureIter measure, NoteIter note) const {
            {
                const auto& notes {measure->instruments.at(instrument)};

                if (note != notes.end()) {
                    const auto next_note {std::next(note)};

                    if (next_note != notes.end()) {
                        if (
                            next_note->id == note->id &&
                            next_note->position == note->position + steps(note->value)
                        ) {
                            return std::make_optional<ProvenanceNote<MeasureIter>>(measure, next_note);
                        }
                    }
                }
            }

            if (note_last_in_measure(*measure, *note)) {
                const auto next_measure {std::next(measure)};

                if (next_measure != measures.end() && next_measure->equal_signature(*measure)) {
                    const auto notes {next_measure->instruments.find(instrument)};

                    if (notes == next_measure->instruments.end()) {
                        return std::nullopt;
                    }

                    const auto next_note {
                        std::ranges::find_if(next_measure->instruments.at(instrument), [next_measure, note](const auto& note_) {
                            return note_.id == note->id && note_first_in_measure(*next_measure, note_);
                        })
                    };

                    if (next_note != notes->second.end()) {
                        return std::make_optional<ProvenanceNote<MeasureIter>>(next_measure, next_note);
                    }
                }
            }

            return std::nullopt;
        }

        template<std::contiguous_iterator MeasureIter>
        std::optional<ProvenanceNote<MeasureIter>> check_note_has_previous(syn::InstrumentId instrument, const ProvenanceNote<MeasureIter>& provenance_note) const {
            return check_note_has_previous(instrument, provenance_note.measure(), provenance_note.note());
        }

        template<std::contiguous_iterator MeasureIter>
        std::optional<ProvenanceNote<MeasureIter>> check_note_has_previous(syn::InstrumentId instrument, MeasureIter measure, NoteIter note) const {
            {
                const auto& notes {measure->instruments.at(instrument)};

                if (note != notes.begin()) {
                    const auto previous_note {std::prev(note)};

                    if (
                        previous_note->id == note->id &&
                        previous_note->position + steps(previous_note->value) == note->position
                    ) {
                        return std::make_optional<ProvenanceNote<MeasureIter>>(measure, previous_note);
                    }
                }
            }

            if (note_first_in_measure(*measure, *note) && measure != measures.begin()) {
                const auto previous_measure {std::prev(measure)};

                if (previous_measure->equal_signature(*measure)) {
                    const auto notes {previous_measure->instruments.find(instrument)};

                    if (notes == previous_measure->instruments.end()) {
                        return std::nullopt;
                    }

                    const auto previous_note {
                        std::ranges::find_if(previous_measure->instruments.at(instrument), [previous_measure, note](const auto& note_) {
                            return note_.id == note->id && note_last_in_measure(*previous_measure, note_);
                        })
                    };

                    if (previous_note != notes->second.end()) {
                        return std::make_optional<ProvenanceNote<MeasureIter>>(previous_measure, previous_note);
                    }
                }
            }

            return std::nullopt;
        }
    };

    namespace exec {
        struct Note {
            Note(syn::NoteId id, double loudness, unsigned int position, unsigned int duration)
                : id(id), loudness(loudness), position(position), duration(duration) {}

            syn::NoteId id {};
            double loudness {};
            unsigned int position {};  // Global, in the whole composition
            unsigned int duration {};  // Number of steps
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
        using UnplayedNotes = std_flat_multiset<UnplayedNote>;
        using PlayedNotes = std_flat_multiset<PlayedNote>;

        struct Execution {
            UnplayedNotes notes_unplayed;
            PlayedNotes notes_played;
        };

        using Executions = std::unordered_map<syn::InstrumentId, Execution>;
    }

    class Player {
    public:
        Player() = default;
        Player(synthesizer::Synthesizer& synthesizer, const Composition& composition, std::function<void()> stopped);

        void prepare();
        void start();
        void stop();
        void seek(unsigned int position);

        double elapsed_time() const { return m_elapsed_time; }
        unsigned int position() const { return m_position; }
        bool playing() const { return m_playing; }
        bool in_time() const { return m_in_time; }
        void metronome(bool metronome) { m_metronome = metronome; }

        void update(double dt);
    private:
        void initialize(unsigned int position);
        exec::Executions initialize_executions(unsigned int position) const;
        ConstMeasureIter initialize_measure(unsigned int position) const;
        double initialize_elapsed_time(unsigned int position) const;
        unsigned int initialize_measure_position(unsigned int position) const;
        unsigned int calculate_note_duration(ConstMeasureIter measure, syn::InstrumentId instrument, unsigned int position, unsigned int duration) const;
        static double calculate_note_loudness(ConstMeasureIter measure, syn::InstrumentId instrument, unsigned int position, unsigned int duration);
        static double calculate_step_time(ConstMeasureIter measure, unsigned int measure_position);
        static double calculate_step_time(const Measure& measure, ConstantTempo tempo);
        static double calculate_step_time(const Measure& measure, unsigned int measure_position, VaryingTempo tempo);
        bool finished() const;
        bool no_notes() const;

        synthesizer::Synthesizer* m_synthesizer {};
        const Composition* m_composition {};
        std::function<void()> m_stopped;

        exec::Executions m_executions;
        ConstMeasureIter m_measure;
        double m_accumulator_time {};
        double m_elapsed_time {};
        unsigned int m_position {};  // Global position, like a cursor
        unsigned int m_measure_position {};  // Local position in current measure
        bool m_playing {};
        bool m_metronome {};
        bool m_in_time {true};  // If the player is able to keep up with the piece
    };

    struct SequencerError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
