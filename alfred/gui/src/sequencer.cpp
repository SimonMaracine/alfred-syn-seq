#include "sequencer.hpp"

#include <algorithm>
#include <iterator>
#include <cassert>

void Composition::validate() const {
    for (const auto& [_, notes] : voices) {
        if (
            !std::is_sorted(notes.begin(), notes.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.position < rhs.position;
            })
        ) {
            throw SequencerError("Invalid note sequence");
        }

        for (const Note& note : notes) {
            for (unsigned int steps {}; const Measure& measure : measures) {
                const unsigned int measure_steps {measure.time_signature.measure_steps()};

                if (steps + measure_steps > note.position) {
                    if (note.position + STEP / note.value > steps + measure_steps) {
                        throw SequencerError("Invalid note duration");
                    }

                    break;
                }

                steps += measure_steps;
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
    if (!m_playing || !m_composition || m_measure == m_composition->measures.end()) {
        return;
    }

    if (done()) {
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

        if (std::next(m_measure) != m_composition->measures.end()) {
            m_measure++;
        }
    }

    for (auto& [voice, execution] : m_executions) {
        Execution::Notes::iterator iter;

        for (iter = execution.notes_played.begin(); iter != execution.notes_played.end(); iter++) {
            if (m_position + 1 < iter->position + STEP / iter->value) {
                execution.notes_played.erase(execution.notes_played.begin(), iter);  // Invalidates iter
                break;
            } else if (m_position + 1 == iter->position + STEP / iter->value) {
                m_synthesizer->note_off(iter->name, iter->octave);

                if (std::next(iter) == execution.notes_played.end()) {
                    execution.notes_played.erase(iter);  // Invalidates iter
                    break;
                }
            }
        }

        for (iter = execution.notes_unplayed.begin(); iter != execution.notes_unplayed.end(); iter++) {
            if (m_position < iter->position) {
                execution.notes_unplayed.erase(execution.notes_unplayed.begin(), iter);  // Invalidates iter
                break;
            } else if (m_position == iter->position) {
                m_synthesizer->note_on(iter->name, iter->octave, voice);
                execution.notes_played.push_back(*iter);

                if (std::next(iter) == execution.notes_unplayed.end()) {
                    execution.notes_unplayed.erase(iter);  // Invalidates iter
                    break;
                }
            }
        }
    }
}

void Player::initialize(unsigned int position) {
    m_executions = initialize_executions(position);
    m_elapsed_time = initialize_time(position);
    m_measure_position = initialize_measure_position(position);

    if (!m_composition->measures.empty()) {
        m_measure = m_composition->measures.begin();
    } else {
        m_measure = m_composition->measures.end();
    }
}

Player::Executions Player::initialize_executions(unsigned int position) const {
    Executions executions;

    for (const auto& [voice, notes] : m_composition->voices) {
        for (const Note& note : notes) {
            if (note.position < position) {
                continue;
            }

            executions[voice].notes_unplayed.push_back(note);
        }
    }

    return executions;
}

unsigned int Player::initialize_measure_position(unsigned int position) const {
    for (unsigned int steps {}; const Measure& measure : m_composition->measures) {
        steps += measure.time_signature.measure_steps();

        if (steps >= position) {
            return steps - position;
        }
    }

    throw SequencerError("Measure position initialization");
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

    throw SequencerError("Time initialization");
}

bool Player::done() const {
    return std::all_of(m_executions.begin(), m_executions.end(), [](const auto& execution) {
        return execution.second.notes_unplayed.empty() && execution.second.notes_played.empty();
    });
}
