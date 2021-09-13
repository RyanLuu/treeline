#include "assets/chart.h"

#include "core/types.h"

namespace assets {
static constexpr char ChartRoot[] = "assets/charts/";
static constexpr size_t NumTracks = 6;

void Chart::load(const std::string &filepath) {
    std::ifstream stream((ChartRoot + filepath).c_str(), std::ios::binary);
    if (!stream.is_open()) {
        LOG_ERROR("Failed to open %s", filepath.c_str());
        exit(1);
    }

    stream.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize fileLength = stream.gcount();
    stream.clear();
    stream.seekg(0, std::ios_base::beg);

    std::array<std::uint8_t, HeaderSize> headerData;
    stream.read(reinterpret_cast<char *>(headerData.data()), HeaderSize);
    parseHeader(headerData);

    size_t dataSize = HeaderSize + m_header.length;
    if (dataSize > fileLength) {
        LOG_ERROR("File is too small to hold chart data (%lu > %lu)", dataSize, fileLength);
        exit(1);
    }

    std::vector<std::uint8_t> fileData(m_header.length);
    stream.read(reinterpret_cast<char *>(fileData.data()), m_header.length);
    Result<size_t> parseResult = parse(fileData);
    if (parseResult.isError) {
        // TODO: handle better
        LOG_ERROR("Error parsing chart: %s", parseResult.error.c_str());
    } else if (parseResult.value < fileData.size()) {
        LOG_WARN("Only parsed %lu out of %lu bytes of chart data in %s.", parseResult.value, fileData.size(), filepath.c_str());
    }

    size_t mdSize = fileLength - dataSize;
    if (mdSize > 0) {
        std::string mdData(mdSize, ' ');
        stream.read(reinterpret_cast<char *>(mdData.data()), mdSize);
        LOG_DEBUG("metadata: \n%s", mdData.c_str());
    }

    LOG_INFO("Loaded chart [%s]", filepath.c_str());
    stream.close();
}

void Chart::unload() {}

void Chart::parseHeader(const std::array<std::uint8_t, HeaderSize> &data) {
    m_header.version = data[0];
    m_header.length = (data[1] << 16) | (data[2] << 8) | data[3];
    LOG_DEBUG("  v%hu | %u bytes", m_header.version, m_header.length);
}

Result<size_t> Chart::parse(const std::vector<std::uint8_t> &data) {
    size_t offset = 0;
    m_meter = 4;
    m_tempo = 500000;
    m_time = 0;  // current time in usec
    while (offset < data.size()) {
        Result<size_t> parseResult;

        // read delta time
        uint32_t deltaTime;  // in ticks
        parseResult = parseVLQ(data, offset, deltaTime);
        if (parseResult.isError) {
            return errorResult<size_t>("Failed to parse delta time: " + parseResult.error);
        }
        offset += parseResult.value;
        m_time += uint32_t(tickTime(m_tempo) * deltaTime);

        // read event
        parseResult = parseEvent(data, offset);
        if (parseResult.isError) {
            return errorResult<size_t>("Failed to parse event: " + parseResult.error);
        }
        offset += parseResult.value;
    }
    return offset;
}

float Chart::tickTime(uint32_t tempo, uint8_t ticksPerQuarter) {
    return static_cast<float>(tempo) / ticksPerQuarter;
}

Result<size_t> Chart::parseEvent(const std::vector<std::uint8_t> &data, size_t offset) {
    size_t length = 1;
    if (offset >= data.size()) {
        return errorResult<size_t>("Event type out of bounds");
    }
    switch (static_cast<EventType>(data[offset++])) {
        case EventType::TIME: {
            Result<size_t> parseResult = parse32(data, offset, m_tempo);
            if (parseResult.isError) {
                return errorResult<size_t>("Failed to parse tempo: " + parseResult.error);
            }
            if (offset >= data.size()) {
                return errorResult<size_t>("Meter information out of bounds");
            }
            m_meter = data[offset];
            length += 5;
            LOG_DEBUG("time event %u %hu (t=%u)", m_tempo, m_meter, m_time);
        } break;
        case EventType::NOTE: {
            if (offset >= data.size()) {
                return errorResult<size_t>("Note value out of bounds");
            }
            uint8_t value = data.at(offset);
            if (value > NumTracks) {
                return errorResult<size_t>("Invalid note value (" + std::to_string(value) + ")");
            }
            length += 1;
            m_notes.emplace_back(Note{.value = value, .time = m_time});
            LOG_DEBUG("note event %hu (t=%u)", value, m_time);
        } break;
        case EventType::HOLD_HEAD: {
        } break;
        case EventType::HOLD_TAIL: {
        } break;
        case EventType::END: {
            LOG_DEBUG("end event (t=%u)", m_time);
            m_length = m_time;
        } break;
    }
    return length;
}

Result<size_t> Chart::parse32(const std::vector<std::uint8_t> &data, size_t offset, uint32_t &value) {
    if (offset + 3 >= data.size()) {
        return errorResult<size_t>("Out of bounds");
    }
    value = data[offset] << 24 |
            data[offset + 1] << 16 |
            data[offset + 2] << 8 |
            data[offset + 3];
    return 4;
}

Result<size_t> Chart::parseVLQ(const std::vector<std::uint8_t> &data, size_t offset, uint32_t &value) {
    value = 0;
    size_t length = 1;
    while (data[offset] >= 0x80) {
        value |= data[offset] & 0x7F;
        value <<= 7;
        offset++;
        length++;
        if (offset >= data.size()) {
            return errorResult<size_t>("Out of bounds (parsed " + std::to_string(offset - 1) + " bytes)");
        }
        if (length > 4) {
            return errorResult<size_t>("Length exceeds 4 bytes");
        }
    }
    value |= data.at(offset);
    return length;
}

Assets<Chart> g_charts;

}  // namespace assets
