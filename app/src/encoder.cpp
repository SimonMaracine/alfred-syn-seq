#include "encoder.hpp"

#include <sstream>
#include <cstdint>

#include <alfred/audio.hpp>
#include <alfred/math.hpp>

// https://en.wikipedia.org/wiki/WAV

namespace encoder {
    struct RiffChunk {
        std::uint8_t file_type_bloc_id[4] { 'R', 'I', 'F', 'F' };
        std::uint32_t file_size {};
        std::uint8_t file_format_id[4] { 'W', 'A', 'V', 'E' };
    };

    struct FormatChunk {
        std::uint8_t format_block_id[4] { 'f', 'm', 't', ' ' };
        std::uint32_t block_size {};
        std::uint16_t audio_format {};
        std::uint16_t number_channels {};
        std::uint32_t frequency {};
        std::uint32_t bytes_per_sec {};
        std::uint16_t bytes_per_block {};
        std::uint16_t bits_per_sample {};
    };

    struct DataChunk {
        std::uint8_t data_block_id[4] { 'd', 'a', 't', 'a' };
        std::uint32_t data_size {};
    };

    enum AudioFormat : std::uint16_t {
        PCM = 1
    };

    utility::Buffer encode_wav(std::size_t count, const double* samples) {
        static constexpr std::uint16_t number_channels = 1;
        static constexpr std::uint32_t frequency = audio::SAMPLE_FREQUENCY;
        static constexpr std::uint16_t bits_per_sample = audio::BITS_PER_SAMPLE;
        static constexpr std::uint16_t bytes_per_block = number_channels * (bits_per_sample / 8);
        static constexpr std::uint32_t bytes_per_sec = frequency * bytes_per_block;

        const std::size_t DataSize = count * bytes_per_block;

        RiffChunk riff_chunk;
        riff_chunk.file_size = std::uint32_t(sizeof(RiffChunk) + sizeof(FormatChunk) + sizeof(DataChunk) + DataSize);

        FormatChunk format_chunk;
        format_chunk.block_size = sizeof(FormatChunk) - 8;
        format_chunk.audio_format = PCM;
        format_chunk.number_channels = number_channels;
        format_chunk.frequency = frequency;
        format_chunk.bytes_per_sec = bytes_per_sec;
        format_chunk.bytes_per_block = bytes_per_block;
        format_chunk.bits_per_sample = bits_per_sample;

        DataChunk data_chunk;
        data_chunk.data_size = std::uint32_t(DataSize);

        std::ostringstream stream {std::ios_base::binary};

        stream.write(reinterpret_cast<char*>(&riff_chunk), sizeof(RiffChunk));
        stream.write(reinterpret_cast<char*>(&format_chunk), sizeof(FormatChunk));
        stream.write(reinterpret_cast<char*>(&data_chunk), sizeof(DataChunk));

        for (std::size_t i {}; i < count; i++) {
            const audio::Resolution sample {
                math::encode_sample<audio::Resolution>(math::clamp_sample(samples[i]))
            };

            stream.write(reinterpret_cast<const char*>(&sample), sizeof(audio::Resolution));
        }

        if (stream.fail()) {
            throw EncoderError("Error writing to stream");
        }

        return stream.str();
    }
}
