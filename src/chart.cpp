#include "chart.h"

#include <fstream>
#include <iostream>
#include <SDL2/SDL.h>

static constexpr char ChartRoot[] = "assets/charts/";

void Chart::load(const std::string &filepath) {
    m_chartFile.open((ChartRoot + filepath).c_str(), std::ios::in);
    assert(m_chartFile.is_open());  // unable to open file
    std::string line;
    SDL_Log("Loaded chart [%s]", filepath.c_str());
    while (std::getline(m_chartFile, line)) {

    }
    
}

void Chart::unload() {
    m_chartFile.close();
}
