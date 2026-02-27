#include "sequencer.hpp"

#include <utility>
#include <cmath>
#include <cassert>

#include "alfred/instrument.hpp"

namespace seq {
    unsigned int Composition::size() const {
        unsigned int steps {};

        for (const Measure& measure : measures) {
            steps += measure.time_signature.measure_steps();
        }

        return steps;
    }

    bool Composition::note_first_in_measure(const Measure&, const Note& note) {
        return note.position == 0;
    }

    bool Composition::note_last_in_measure(const Measure& measure, const Note& note) {
        return note.position + steps(note.value) == measure.time_signature.measure_steps();
    }

    Player::Player(synthesizer::Synthesizer& synthesizer, const Composition& composition, std::function<void()> stopped)
        : m_synthesizer(&synthesizer), m_composition(&composition), m_stopped(std::move(stopped)) {
        initialize(0);
    }

    void Player::prepare() {
        seek(m_position);
    }

    void Player::start() {
        m_playing = true;
    }

    void Player::stop() {
        m_playing = false;
        m_in_time = true;
        m_synthesizer->silence();
    }

    void Player::seek(unsigned int position) {
        m_position = position;

        m_playing = false;
        m_in_time = true;
        m_synthesizer->silence();
        m_accumulator_time = 0.0;

        m_executions.clear();
        initialize(m_position);

        m_stopped();
    }

    void Player::update(double dt) {
        if (!m_playing || !m_composition) {
            return;
        }

        if (finished()) {
            if (!no_notes()) {
                throw SequencerError("Invalid remaining notes");
            }

            m_playing = false;
            m_in_time = true;
            m_stopped();

            return;
        }

        m_accumulator_time += dt;

        const double step_time {m_measure->time_signature.step_time(m_measure->tempo)};

        // Advance with maximum one step per frame
        // If the frame time is larger than the step time, then the player will simply play at a lower and inconsistent speed
        if (m_accumulator_time >= step_time) {
            m_accumulator_time -= step_time;
            m_elapsed_time += step_time;
            m_position++;
            m_measure_position++;
        }

        m_in_time = m_accumulator_time < step_time;

        if (m_measure_position == m_measure->time_signature.measure_steps()) {
            m_measure_position = 0;
            m_measure++;
        }

        for (auto& [instrument, execution] : m_executions) {
            for (auto note {execution.notes_played.begin()}; note != execution.notes_played.end(); note++) {
                if (m_position < note->position + note->duration) {
                    execution.notes_played.erase(execution.notes_played.begin(), note);
                    break;
                }

                if (m_position == note->position + note->duration) {
                    m_synthesizer->note_off(note->id, instrument);

                    if (std::next(note) == execution.notes_played.end()) {
                        execution.notes_played.erase(execution.notes_played.begin(), std::next(note));
                        break;
                    }
                }
            }

            for (auto note {execution.notes_unplayed.begin()}; note != execution.notes_unplayed.end(); note++) {
                if (m_position < note->position) {
                    execution.notes_unplayed.erase(execution.notes_unplayed.begin(), note);
                    break;
                }

                if (m_position == note->position) {
                    m_synthesizer->note_on(note->id, instrument, note->loudness);

                    execution.notes_played.emplace(
                        note->id,
                        note->loudness,
                        note->position,
                        note->duration
                    );

                    if (std::next(note) == execution.notes_unplayed.end()) {
                        execution.notes_unplayed.erase(execution.notes_unplayed.begin(), std::next(note));
                        break;
                    }
                }
            }
        }
    }

    void Player::initialize(unsigned int position) {
        m_executions = initialize_executions(position);
        m_measure = initialize_measure(position);
        m_elapsed_time = initialize_elapsed_time(position);
        m_measure_position = initialize_measure_position(position);
    }

    exec::Executions Player::initialize_executions(unsigned int position) const {
        exec::Executions executions;
        unsigned int steps {};

        std::vector<ProvenanceNote<ConstMeasureIter>> processed_notes;
        Composition cloned_composition;

        // By default, simply reference the composition pointer
        const Composition* composition {m_composition};

        // Make a full copy of the composition, add metronome and use a reference to the copied
        if (m_metronome) {
            cloned_composition = *m_composition;

            for (auto measure {cloned_composition.measures.begin()}; measure != cloned_composition.measures.end(); measure++) {
                for (unsigned int i {}; i < measure->time_signature.measure_steps(); i += seq::steps(measure->time_signature.value())) {
                    measure->instruments[instrument::Metronome::static_id()].emplace(
                        i == 0 ? syn::note(syn::B, syn::Octave5) : syn::note(syn::A, syn::Octave5),
                        Sixteenth,
                        Loudness::Fortississimo,
                        i
                    );
                }
            }

            composition = &cloned_composition;
        }

        for (auto measure {composition->measures.begin()}; measure != composition->measures.end(); measure++) {
            for (const auto& [instrument, notes] : measure->instruments) {
                for (auto note {notes.begin()}; note != notes.end(); note++) {
                    if (steps + note->position < position) {
                        continue;
                    }

                    if (
                        std::ranges::find_if(processed_notes, [measure, note](const auto& provenance_note) {
                            return provenance_note.measure() == measure && provenance_note.note() == note;
                        }
                    ) != processed_notes.end()) {
                        continue;
                    }

                    NoteIter current_note {note};
                    unsigned int duration {seq::steps(current_note->value) - current_note->delay};

                    while (current_note->legato) {
                        const auto next_note {
                            composition->check_note_has_next<ConstMeasureIter>(instrument, ProvenanceNote(measure, current_note))
                        };

                        assert(next_note);

                        duration += seq::steps(next_note->note()->value);

                        processed_notes.push_back(*next_note);
                        current_note = next_note->note();
                    }

                    executions[instrument].notes_unplayed.emplace(
                        note->id,
                        loudness(note->loudness),
                        steps + note->position + note->delay,
                        calculate_note_duration(measure, instrument, duration)
                    );
                }
            }

            steps += measure->time_signature.measure_steps();
        }

        return executions;
    }

    ConstMeasureIter Player::initialize_measure(unsigned int position) const {
        unsigned int steps {};
        ConstMeasureIter measure;

        for (measure = m_composition->measures.begin(); measure != m_composition->measures.end(); measure++) {
            steps += measure->time_signature.measure_steps();

            if (steps > position) {
                return measure;
            }
        }

        return measure;
    }

    double Player::initialize_elapsed_time(unsigned int position) const {
        unsigned int steps {};
        double time {};

        for (const Measure& measure : m_composition->measures) {
            steps += measure.time_signature.measure_steps();

            if (steps > position) {
                const unsigned int last_steps {measure.time_signature.measure_steps() - (steps - position)};

                time += double(last_steps) * measure.time_signature.step_time(measure.tempo);

                return time;
            }

            time += double(measure.time_signature.measure_steps()) * measure.time_signature.step_time(measure.tempo);
        }

        return time;
    }

    unsigned int Player::initialize_measure_position(unsigned int position) const {
        unsigned int steps {};

        for (const Measure& measure : m_composition->measures) {
            steps += measure.time_signature.measure_steps();

            if (steps > position) {
                return measure.time_signature.measure_steps() - (steps - position);
            }
        }

        return steps;
    }

    unsigned int Player::calculate_note_duration(ConstMeasureIter measure, syn::InstrumentId instrument, unsigned int duration) const {
        const double step_time {measure->time_signature.step_time(measure->tempo)};
        const double release_time {m_synthesizer->get_instrument(instrument).release_duration()};
        const unsigned int release_duration {static_cast<unsigned int>(std::ceil(release_time / step_time))};

        if (release_duration > duration) {
            return MIN_DURATION;
        }

        return std::max(duration - release_duration, MIN_DURATION);
    }

    bool Player::finished() const {
        return m_measure == m_composition->measures.end();
    }

    bool Player::no_notes() const {
        return std::ranges::all_of(m_executions, [](const auto& execution) {
            return execution.second.notes_unplayed.empty() && execution.second.notes_played.empty();
        });
    }
}
