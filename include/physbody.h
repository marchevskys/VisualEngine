#ifndef PHYSBODY_H
#define PHYSBODY_H
#include <glm/glm.hpp>
#include <vector>
using mat4d = glm::mat<4, 4, double, glm::highp>;
using vec4d = glm::vec<4, double, glm::highp>;
using vec3d = glm::vec<3, double, glm::highp>;
using vec2d = glm::vec<2, double, glm::highp>;

// PHYSICS WORLD
class PhysWorld {
    class NewtonWorld *m_world;

  public:
    PhysWorld();
    ~PhysWorld();
    inline NewtonWorld *get() const { return m_world; }
    void update(const double dt);
};

// COLLISION SHAPE
class CollisionShape {
  protected:
    class NewtonCollision *m_collision = nullptr;
    static int shapeCounter;

  public:
    CollisionShape(const PhysWorld &world);
    CollisionShape(CollisionShape &&other);
    virtual ~CollisionShape() = 0;
    const NewtonCollision *get() { return m_collision; }
};

class CollisionSphere : public CollisionShape {
  public:
    CollisionSphere(const PhysWorld &world, double radius = 1.0);
};

class CollisionIcosahedron : public CollisionShape {
  public:
    CollisionIcosahedron(const PhysWorld &world, double radius = 1.0);
};

class CollisionCuboid : public CollisionShape {
  public:
    CollisionCuboid(const PhysWorld &world, vec3d dimensions);
};

class CollisionConvex : public CollisionShape {
  public:
    CollisionConvex(const PhysWorld &world, const std::vector<double> points);
};

// PHYSICS BODY
class PhysBody {
  public:
    // zero mass by default (body is static)
    PhysBody();
    PhysBody(const PhysWorld &world, CollisionShape &shape, const double m = 0.0, const vec3d ixyz = {0.0, 0.0, 0.0});
    PhysBody(const PhysWorld &world, CollisionShape &&shape, const double m = 0.0, const vec3d ixyz = {0.0, 0.0, 0.0});
    PhysBody(const PhysBody &other);
    PhysBody(PhysBody &&other) noexcept;

    void setPos(vec3d pos);
    void setMatrix(mat4d mat);
    void setVelocity(vec3d vel);
    void setOmega(vec3d vel);

    vec3d getPos();
    mat4d getMatrix();
    vec3d getVelocity();
    vec3d getOmega();

    void addForce(vec3d force);
    void addTorque(vec3d torque);

    void setMass(double mass, vec3d ixyz);
    void setMass(double mass); // scales moment of inertia

    void setBallJoint(const PhysBody &body, vec3d pos);
    void setSlideJoint(const PhysBody &body, vec3d pos, vec3d dir);
    void setCorkscrewJoint(const PhysBody &body, vec3d pos, vec3d dir);
    void setHingeJoint(const PhysBody &body, vec3d pos, vec3d dir);
    void setUniversalJoint(const PhysBody &body, vec3d pivot, vec3d pin0, vec3d pin1);
    void setUserJoint(const PhysBody &body);

    ~PhysBody();
    struct Data {
        vec3d force = {0, 0, 0};
        vec3d torque = {0, 0, 0};
    } data;

  protected:
    class NewtonBody *m_body = nullptr;

  private:
    void createBody(const PhysWorld &world, CollisionShape &shape, const double mass, const vec3d ixyz);
    void turnOffDefaultResistance();
};

class Joint {
  public:
    struct Data {
    } data;
};

#endif // PHYSBODY_H
