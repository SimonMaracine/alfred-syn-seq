#include "sequencer.hpp"

#include <algorithm>
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
                const unsigned int measure_steps {measure.time_signature.beats() * (Sixteenth / measure.time_signature.value())};

                if (steps + measure_steps == note.position) {  // FIXME
                    break;
                } else if (steps + measure_steps > note.position) {
                    if (note.position + Sixteenth / note.value > steps + measure_steps) {
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

void Player::start() {
    m_playing = true;
}

void Player::stop() {
    m_playing = false;
    m_synthesizer->silence();
}

void Player::seek(unsigned int position) {
    stop();
    m_executions.clear();
    m_composition->validate();
    initialize(position);
    m_position = position;
}

void Player::reload() {
    seek(m_position);
}

void Player::update(double dt) {
    if (!m_playing || !m_composition) {
        return;
    }

    if (done()) {
        stop();
        return;
    }

    // m_elapsed_time += dt;
    m_accumulator_time += dt;

    unsigned int steps {};
    double step_time {};

    for (auto iter {m_composition->measures.begin()}; iter != m_composition->measures.end(); iter++) {
        steps += iter->time_signature.measure_steps();

        if (steps >= m_position) {
            step_time = iter->time_signature.step_time(iter->tempo);
            break;
        }
    }

    // Advance with maximum one step per frame
    // If the frame time is larger than the step time, then the player will simply play at a lower and inconsistent speed
    if (m_accumulator_time >= step_time) {
        m_accumulator_time -= step_time;
        m_elapsed_time += step_time;
        m_position++;
    }

    for (auto& [voice, execution] : m_executions) {
        Execution::Notes::iterator iter;

        for (iter = execution.notes_played.begin(); iter != execution.notes_played.end(); iter++) {
            const auto [note, time] {*iter};

            if (m_elapsed_time < time.end) {
                execution.notes_played.erase(execution.notes_played.begin(), iter);  // Invalidates iter
                break;
            }

            m_synthesizer->note_off(note.name, note.octave);

            printf("OFF\n");

            if (std::next(iter) == execution.notes_played.end()) {
                execution.notes_played.erase(iter);  // Invalidates iter
                break;
            }
        }

        for (iter = execution.notes_unplayed.begin(); iter != execution.notes_unplayed.end(); iter++) {
            const auto [note, time] {*iter};

            if (m_elapsed_time < time.begin) {
                execution.notes_unplayed.erase(execution.notes_unplayed.begin(), iter);  // Invalidates iter
                break;
            }

            m_synthesizer->note_on(note.name, note.octave, voice);
            execution.notes_played.emplace_back(note, time);

            if (std::next(iter) == execution.notes_unplayed.end()) {
                execution.notes_unplayed.erase(iter);  // Invalidates iter
                break;
            }

            printf("ON\n");
        }
    }
}

void Player::initialize(unsigned int position) {
    m_executions = initialize_executions(position);
    m_elapsed_time = initialize_time(position);
}

Player::Executions Player::initialize_executions(unsigned int position) const {
    Executions executions;

    for (const auto& [voice, notes] : m_composition->voices) {
        for (const Note& note : notes) {
            if (note.position < position) {
                continue;
            }

            executions[voice].notes_unplayed.push_back(initialize_note(note));
        }
    }

    return executions;
}

Player::Execution::Notes::value_type Player::initialize_note(const Note& note) const {
    unsigned int steps {};
    double time {};

    for (const Measure& measure : m_composition->measures) {
        const unsigned int measure_steps {measure.time_signature.measure_steps()};

        if (steps + measure_steps >= note.position) {
            const unsigned int last_steps {steps + measure_steps - note.position};

            steps += measure_steps - last_steps;
            time += double(measure_steps - last_steps) * measure.time_signature.step_time(measure.tempo);

            Execution::Time execution_time;
            execution_time.begin = time;
            execution_time.end = time + double(Sixteenth / note.value) * measure.time_signature.step_time(measure.tempo);

            return std::make_pair(note, execution_time);
        }

        steps += measure_steps;
        time += double(measure_steps) * measure.time_signature.step_time(measure.tempo);
    }

    throw SequencerError("Note initialization");
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
