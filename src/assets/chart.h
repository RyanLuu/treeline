#pragma once

#include <SDL.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "assets/assets.h"
#include "assets/texture.h"
#include "components/render.h"
#include "components/tags.h"
#include "components/transform.h"
#include "core/ecs.h"
#include "core/logging.h"
#include "core/types.h"

namespace assets {

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
    void load(const std::string &filepath) override;

    std::vector<Note> getNotes() {
        return m_notes;
    }

    void unload() override;

   private:
    static constexpr size_t HeaderSize = 4;
    void parseHeader(const std::array<std::uint8_t, HeaderSize> &data);
    Result<size_t> parse(const std::vector<std::uint8_t> &data);
    float tickTime(uint32_t tempo, uint8_t ticksPerQuarter = 96);
    Result<size_t> parseEvent(const std::vector<std::uint8_t> &data, size_t offset);
    Result<size_t> parse32(const std::vector<std::uint8_t> &data, size_t offset, uint32_t &value);

    Result<size_t> parseVLQ(const std::vector<std::uint8_t> &data, size_t offset, uint32_t &value);

    ChartHeader m_header;
    std::vector<Note> m_notes;
    uint32_t m_length;  // length of chart in usec

    // temporary "globals" for parsing
    uint32_t m_tempo;  // in usec / quarternote
    uint8_t m_meter;   // time signature over 4
    uint32_t m_time;   // current time in usec
};

extern Assets<Chart> g_charts;

}  // namespace assets

