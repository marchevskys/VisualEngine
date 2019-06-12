#ifndef ENTITYCREATOR_H
#define ENTITYCREATOR_H

#include <memory>
class EntityCreator
{
public:
    EntityCreator();
    void generateAsteroid();
    void createSpaceShip();
private:
    std::unique_ptr<class CachedData> m_data;
};

#endif // ENTITYCREATOR_H
