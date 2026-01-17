#include "sequencer.hpp"

#include <algorithm>
#include <iterator>
#include <cassert>

namespace seq {
    void Composition::validate() const {
        for (const Measure& measure : measures) {
            for (const auto& [_, notes] : measure.voices) {
                for (const Note& note : notes) {
                    if (note.position + STEP / note.value > measure.time_signature.measure_steps()) {
                        throw SequencerError("Invalid note duration");
                    }
                }
            }
        }
    }

    Player::Player(synthesizer::Synthesizer& synthesizer, const Composition& composition)
        : m_synthesizer(&synthesizer), m_composition(&composition) {
        m_composition->validate();
        initialize(0);
    }

    void Player::prepare() {
        seek(m_position);
    }

    void Player::start() {
        m_playing = true;
    }

    void Player::stop() {
        seek(m_position);
    }

    void Player::seek(unsigned int position) {
        m_position = position;

        m_playing = false;
        m_synthesizer->silence();
        m_accumulator_time = 0.0;

        m_executions.clear();
        m_composition->validate();
        initialize(m_position);
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

        if (m_measure_position == m_measure->time_signature.measure_steps()) {
            m_measure_position = 0;
            m_measure++;
        }

        for (auto& [voice, execution] : m_executions) {
            for (auto note {execution.notes_played.begin()}; note != execution.notes_played.end(); note++) {
                if (m_position + 1 < note->position + note->duration) {
                    execution.notes_played.erase(execution.notes_played.begin(), note);
                    break;
                } else if (m_position + 1 == note->position + note->duration) {
                    m_synthesizer->note_off(note->name, note->octave);

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
                } else if (m_position == note->position) {
                    m_synthesizer->note_on(note->name, note->octave, voice);

                    execution.notes_played.emplace(
                        note->name,
                        note->octave,
                        note->position,
                        note->duration,
                        note->tempo,
                        note->time_signature
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
        m_executions = initialize_executions(position);  // FIXME
        m_elapsed_time = initialize_time(position);
        m_measure_position = initialize_measure_position(position);

        if (!m_composition->measures.empty()) {
            m_measure = m_composition->measures.begin();
        } else {
            m_measure = m_composition->measures.end();
        }
    }

    exec::Executions Player::initialize_executions(unsigned int position) const {
        exec::Executions executions;

        for (unsigned int steps {}; const Measure& measure : m_composition->measures) {
            for (const auto& [voice, notes] : measure.voices) {
                for (const Note& note : notes) {
                    if (note.position < position) {
                        continue;
                    }

                    executions[voice].notes_unplayed.emplace(
                        note.name,
                        note.octave,
                        steps + note.position,
                        STEP / note.value,  // TODO implement legatos
                        measure.tempo,
                        measure.time_signature
                    );
                }
            }

            steps += measure.time_signature.measure_steps();
        }

        return executions;
    }

    unsigned int Player::initialize_measure_position(unsigned int position) const {
        for (unsigned int steps {}; const Measure& measure : m_composition->measures) {
            steps += measure.time_signature.measure_steps();

            if (steps >= position) {
                return measure.time_signature.measure_steps() - steps - position;
            }
        }

        return 0;
    }

    double Player::initialize_time(unsigned int position) const {
        unsigned int steps {};
        double time {};

        for (const Measure& measure : m_composition->measures) {
            const unsigned int measure_steps {measure.time_signature.measure_steps()};

            if (steps + measure_steps >= position) {
                const unsigned int last_steps {steps + measure_steps - position};

                steps += measure_steps - last_steps;
                time += double(measure_steps - last_steps) * measure.time_signature.step_time(measure.tempo);

                return time;
            }

            steps += measure_steps;
            time += double(measure_steps) * measure.time_signature.step_time(measure.tempo);
        }

        return 0.0;
    }

    bool Player::finished() const {
        return m_measure == m_composition->measures.end();
    }

    bool Player::no_notes() const {
        return std::all_of(m_executions.begin(), m_executions.end(), [](const auto& execution) {
            return execution.second.notes_unplayed.empty() && execution.second.notes_played.empty();
        });
    }
}
