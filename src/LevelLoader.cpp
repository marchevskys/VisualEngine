#include "LevelLoader.h"
#include "../nlohman_json/json.hpp"
#include "game.h"
#include <algorithm>
#include <fstream>
#include "logger.h"
#include <iostream>
#include "generator.h"

//for convinience
using json = nlohmann::json;

LevelLoader::LevelLoader(Game& game)
    : m_game(game)
    , m_observers()
    , m_levelinfo{}
{
}

LevelLoader::~LevelLoader() {
    m_observers.clear();
}

void LevelLoader::registerLevelObserver(LevelLoaderObserver& observer) {
    if (m_observers.cend() == std::find(m_observers.cbegin(), m_observers.cend(), &observer)) {
        m_observers.push_back(&observer);
    }
}

void LevelLoader::unregisterLevelObserver(LevelLoaderObserver& observer) {
    auto const it = std::find(m_observers.cbegin(), m_observers.cend(), &observer);
    if (it != m_observers.end()) {
        m_observers.erase(it);
    }
}

ex::Entity LevelLoader::tryLoadLevel(const std::string& filename) {
    json level = json::parse(readfile(filename));
    std::cout << "read: " << level << "\n"; 
    m_levelinfo.level = level.value("level", -1);
    m_levelinfo.difficulcity = level.value("difficulcity", -1);
    m_levelinfo.asteroid_density = level.value("asteroiddensity", -1);
    //std::cout << "level: " << m_levelinfo.level << "\n"
    //    << "diff: " << m_levelinfo.difficulcity << "\n"
    //    << "asteroids: " << m_levelinfo.asteroid_density << "\n";
    auto spaceshippos = level["spaceshipposition"];
    auto voxel = spaceshippos["voxel"];
    auto position = spaceshippos["pos"];
    const glm::dvec3 shipPosition(position[0], position[1], position[2]);
    //std::cout << "position: " << position[0] << ", "
    //    << position[1] << ", "
    //    << position[2] << "\n";
    
    // TODO: calculate amount of asteroids basing on given density
    auto shipEntity = Generator::generateShip(m_game, shipPosition);
    for (int i = 0; i < m_levelinfo.asteroid_density; i++) {
        Generator::generateAsteroid(m_game, { voxel[0], voxel[1], voxel[2] });
    }
    return shipEntity;
}

std::string LevelLoader::readfile(const std::string& filename) {
    std::ifstream inputstream(filename);
    if (inputstream.is_open()) {
        std::string buffer;
        std::string filecontains;
        while (inputstream.good()) {
            std::getline(inputstream, buffer);
            filecontains.append(buffer);
        }
        return filecontains;
    }
    else
    {
        DLOG("Error: ", "no file ", filename.c_str());
    }
    return "";
}

void LevelLoader::sendPercentageUpdate(int percents)
{
    for (auto* observer : m_observers) {
        observer->updatePercentage(percents);
    }
}



