#pragma once

#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <string>

#include "assets.h"

static constexpr char ChartRoot[] = "assets/charts/";

class Chart : public Asset {
    public:
        void load(const std::string &filepath) override {
            m_chartFile.open((ChartRoot + filepath).c_str(), std::ios::in);
            assert(m_chartFile.is_open());  // unable to open file
            std::string line;
            while (std::getline(m_chartFile, line)) {

            }
            SDL_Log("Loaded chart [%s]", filepath.c_str());
        }
        void unload() override {
            m_chartFile.close();
        }
    private:
        std::fstream m_chartFile;
};

extern Assets<Chart> g_charts;

