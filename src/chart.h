#pragma once

#include <SDL.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "src/assets.h"
#include "src/logging.h"

namespace chart {

static constexpr char ChartRoot[] = "assets/charts/";
static constexpr size_t HeaderSize = 4;
static constexpr size_t NumTracks = 4;

enum class EventType : uint8_t {
    TIME = 0x00,
    NOTE = 0x10,
    HOLD_HEAD = 0x11,
    HOLD_TAIL = 0x12,
    END = 0xFF
};

struct ChartHeader {
    uint8_t version;  // currently version 0
    uint32_t length;  // number of bytes
};

struct Note {
    uint8_t value;    // 0x00 corresponds to Key::NOTE_A, etc.
    uint32_t time;    // onset in usec
    uint32_t length;  // duration in usec (0 if tap note)
};

class Chart : public Asset {
 public:
    void load(const std::string &filepath) override {
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
            LOG_ERROR("Chart::parse: file is too small to hold data (%lu > %lu)", dataSize, fileLength);
            exit(1);
        }

        std::vector<std::uint8_t> fileData(m_header.length);
        stream.read(reinterpret_cast<char *>(fileData.data()), m_header.length);
        parse(fileData);

        size_t mdSize = fileLength - dataSize;
        if (mdSize > 0) {
            std::string mdData(mdSize, ' ');
            stream.read(reinterpret_cast<char *>(mdData.data()), mdSize);
            LOG_INFO("metadata: \n%s", mdData.c_str());
        }

        LOG_INFO("Loaded chart [%s]", filepath.c_str());
        stream.close();
    }
    void unload() override {}

 private:
    void parseHeader(const std::array<std::uint8_t, HeaderSize> &data) {
        m_header.version = data[0];
        m_header.length = (data[1] << 16) | (data[2] << 8) | data[3];
        LOG_INFO("  v%hu | %u bytes", m_header.version, m_header.length);
    }

    void parse(const std::vector<std::uint8_t> &data) {
        size_t offset = 0;
        m_meter = 4;
        m_tempo = 500000;
        m_time = 0;  // current time in usec
        while (offset < data.size()) {
            uint32_t deltaTime;  // in ticks
            offset += parseVLQ(data, offset, deltaTime);
            offset += parseEvent(data, offset);
            m_time += uint32_t(tickTime(m_tempo) * deltaTime);
        }
    }

    float tickTime(uint32_t tempo, uint8_t ticksPerQuarter = 96) {
        return static_cast<float>(tempo) / ticksPerQuarter;
    }

    size_t parseEvent(const std::vector<std::uint8_t> &data, size_t offset) {
        size_t length = 1;
        switch (static_cast<EventType>(data.at(offset++))) {
            case EventType::TIME: {
                offset += parse32(data, offset, m_tempo);
                m_meter = data.at(offset);
                length += 5;
                LOG_INFO("time event %u %hu (t=%u)", m_tempo, m_meter, m_time);
            } break;
            case EventType::NOTE: {
                uint8_t value = data.at(offset);
                length += 1;
                m_notes.emplace_back(Note{.value = value, .time = m_time});
                LOG_INFO("note event %hu (t=%u)", value, m_time);
            } break;
            case EventType::HOLD_HEAD: {
            } break;
            case EventType::HOLD_TAIL: {
            } break;
            case EventType::END: {
                LOG_INFO("end event (t=%u)", m_time);
                m_length = m_time;
            } break;
        }
        return length;
    }

    uint32_t parse32(const std::vector<std::uint8_t> &data, size_t offset,
                     uint32_t &value) {
        value = data.at(offset) << 24 | data.at(offset + 1) << 16 |
                data.at(offset + 2) << 8 | data.at(offset + 3);
        return 4;
    }

    size_t parseVLQ(const std::vector<std::uint8_t> &data, size_t offset,
                    uint32_t &value) {
        value = 0;
        size_t length = 1;
        while (data.at(offset) >= 0x80) {
            value |= data[offset] & 0x7F;
            value <<= 7;
            offset++;
            length++;
        }
        value |= data.at(offset);
        return length;
    }

    ChartHeader m_header;
    std::vector<Note> m_notes;
    uint32_t m_length;  // length of chart in usec

    // temporary "globals" for parsing
    uint32_t m_tempo;  // in usec / quarternote
    uint8_t m_meter;   // time signature over 4
    uint32_t m_time;   // current time in usec
};

extern Assets<Chart> g_charts;

}  // namespace chart

