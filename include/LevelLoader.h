#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "entityx/entityx.h"
/*
 The level description is like following:
 {
    "level": 0,
    "difficulcity": 0,
    "spaceshipposition": {
        "voxel": [0,0,0],
        "pos": [0,0,0]
    },
    "asteroiddensity": 5,   // in percents
    // description for every enemy
    "enemies": [
    {
        "enemy_type": {"amount": 10, "location" : {"voxel" : [0, 1, 1], "position" : {"type": "random"}}},
        "enemy_type": {"amount": 5, "location" : {"voxel" : [1, 1, 1], "position" : {"type": "circle", "center": [-5,35,-26], "radius":"50" }}},
        "enemy_type": {"amount": 5, "location" : {"voxel" : [1, 1, 1], "position" : {"type": "line", "first": [x,y,z], "second": [x,y,z]}}}
    }
    ]
}
*/

class Game;

class LevelLoaderObserver {
public:
    enum class Error {
        InsufficientResources,
        NoFile,
        Unknown
    };
    virtual void updatePercentage(int loaded) = 0;
    virtual void loaded() = 0;
    virtual void onError(Error e) = 0;
};

class LevelLoader {
public:
    struct LevelInfo {
        int level;
        int difficulcity;
        int asteroid_density;
        int enemies_amount;
    };
    LevelLoader(Game& thisGame);
    ~LevelLoader();
    void registerLevelObserver(LevelLoaderObserver& lo);
    void unregisterLevelObserver(LevelLoaderObserver& lo);
    entityx::Entity tryLoadLevel(const std::string& levelfilename);
    LevelInfo getInfo() const { return m_levelinfo; }
private:
    std::string static readfile(const std::string& filename);
    void sendPercentageUpdate(int percents);
    LevelLoader(const LevelLoader& rhs) = delete;
    LevelLoader& operator=(const LevelLoader& rhs) = delete;
    Game& m_game;
    std::vector<LevelLoaderObserver*> m_observers;
    LevelInfo m_levelinfo;  // this may be used for informational purposes
};
