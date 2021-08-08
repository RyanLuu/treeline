#pragma once

#include <fstream>
#include <string>

#include "assets.h"

class Chart : public Asset {
    public:
        void load(const std::string &filepath) override;
        void unload() override;
    private:
        std::fstream m_chartFile;
};

extern Assets<Chart> g_charts;

