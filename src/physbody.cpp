#include "physbody.h"
#include "logger.h"
#include "meshdata.h"

#define _NEWTON_USE_DOUBLE
#include <newton/Newton.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define ptr(data) glm::value_ptr(data);
#define GetBodyData(body) static_cast<PhysBody::Data *>(NewtonBodyGetUserData(body))

using glm::length;
using glm::normalize;
using glm::to_string;
using glm::value_ptr;

static const mat4d Identity(1.0);
int CollisionShape::shapeCounter = 0;

// PHYSICS WORLD
PhysWorld::PhysWorld() {
    m_world = NewtonCreate();
    //DLOG(NewtonGetMaxThreadsCount(m_world));
    //THROW();
    NewtonSetThreadsCount(m_world, 40);
    NewtonSetNumberOfSubsteps(m_world, 8);
    DLOG("world created");
}

PhysWorld::~PhysWorld() {
    NewtonWaitForUpdateToFinish(m_world);
    NewtonDestroyAllBodies(m_world);
    NewtonMaterialDestroyAllGroupID(m_world);
    NewtonDestroy(m_world);
    DLOG("world destroyed");
}

void PhysWorld::update(const double dt) {
    NewtonUpdate(m_world, dt);

    //NewtonUpdateAsync(m_world, dt);

    for (NewtonBody *body = NewtonWorldGetFirstBody(m_world); body != nullptr; body = NewtonWorldGetNextBody(m_world, body)) {
        double mass, Ixx, Iyy, Izz;
        NewtonBodyGetMass(body, &mass, &Ixx, &Iyy, &Izz);
        PhysBody::Data *mydata = GetBodyData(body);
        mydata->torque = {0.0, 0.0, 0.0};                  // reset force to prevent accumulation from previous update
        mydata->force = {0.0, 0.0, 1.0 * -9.80665 * mass}; // G-force // gravity here;
    }
}

// COLLISIONS
CollisionShape::CollisionShape(const PhysWorld &world) {}

CollisionShape::CollisionShape(CollisionShape &&other) { other.m_collision = nullptr; }

CollisionShape::~CollisionShape() {
    if (m_collision)
        NewtonDestroyCollision(m_collision);
}

CollisionSphere::CollisionSphere(const PhysWorld &world, double radius) : CollisionShape(world) {
    m_collision = NewtonCreateSphere(world.get(), radius, CollisionShape::shapeCounter++, nullptr);
}

CollisionCuboid::CollisionCuboid(const PhysWorld &world, vec3d dimensions) : CollisionShape(world) {
    m_collision = NewtonCreateBox(world.get(), dimensions.x, dimensions.y, dimensions.z, CollisionShape::shapeCounter++, nullptr);
}

CollisionIcosahedron::CollisionIcosahedron(const PhysWorld &world, double radius) : CollisionShape(world) {
    THROW("not ready");
    //const auto vertices = Visual::MeshDataPrimitive::icosahedron().getVertices<double>();
    //m_collision = NewtonCreateConvexHull(world.get(), vertices.size() / 3, vertices.data(), sizeof(double) * 3, /*tolerance*/ 0.0,
    //                                     CollisionShape::shapeCounter++, nullptr);
}

CollisionConvex::CollisionConvex(const PhysWorld &world, const std::vector<double> points) : CollisionShape(world) {
    m_collision = NewtonCreateConvexHull(world.get(), points.size() / 3, points.data(), sizeof(double) * 3, /*tolerance*/ 0.0,
                                         CollisionShape::shapeCounter++, nullptr);
}

// PHYSICS BODY
void setForcesAndTorques(const NewtonBody *const body, double timestep, int threadIndex) {

    PhysBody::Data *mydata = GetBodyData(body);

    glm::dvec3 speed; // calculate aerodynamics
    NewtonBodyGetVelocity(body, value_ptr(speed));
    glm::dvec3 aeroForce = -0.005 * glm::length(speed) * speed;
    mydata->force += aeroForce;

    NewtonBodyAddForce(body, value_ptr(mydata->force));
    NewtonBodyAddTorque(body, value_ptr(mydata->torque));
}

void updateTransformCallback(const NewtonBody *const body, const dFloat *const matrix, int threadIndex) {}

void PhysBody::turnOffDefaultResistance() {
    NewtonBodySetLinearDamping(m_body, 0.0);
    double angularDamping = 0.0;
    NewtonBodySetAngularDamping(m_body, &angularDamping);
}

PhysBody::PhysBody() { THROW("Default constructor of Physbody. "
                             "It cannot be deleted due to ECS. "
                             "And I don't know, what to do."); }

PhysBody::PhysBody(const PhysWorld &world, CollisionShape &shape, const double mass, const vec3d ixyz) {
    createBody(world, shape, mass, ixyz);
}

PhysBody::PhysBody(const PhysWorld &world, CollisionShape &&shape, const double mass, const vec3d ixyz) {
    createBody(world, shape, mass, ixyz);
}

void PhysBody::createBody(const PhysWorld &world, CollisionShape &shape, const double mass, const vec3d ixyz) {
    m_body = NewtonCreateDynamicBody(world.get(), shape.get(), value_ptr(Identity));
    setMass(mass, ixyz);
    NewtonBodySetForceAndTorqueCallback(m_body, setForcesAndTorques);
    NewtonBodySetTransformCallback(m_body, updateTransformCallback);
    //NewtonBodySetContinuousCollisionMode(m_body, 1);
    NewtonBodySetUserData(m_body, static_cast<void *>(&data));
    turnOffDefaultResistance();
}

PhysBody::PhysBody(PhysBody &&other) noexcept {
    m_body = other.m_body;
    other.m_body = nullptr;
    data = other.data;
    NewtonBodySetUserData(m_body, static_cast<void *>(&data));
    turnOffDefaultResistance();
}

PhysBody::PhysBody(const PhysBody &other) { THROW("TRY TO COPY PHYSBODY"); }

void PhysBody::setMass(double mass, vec3d ixyz) { NewtonBodySetMassMatrix(m_body, mass, ixyz.x, ixyz.y, ixyz.z); }

void PhysBody::setMass(double mass) {
    double previousMass;
    vec3d ixyz;
    NewtonBodyGetMass(m_body, &previousMass, &ixyz.x, &ixyz.y, &ixyz.z);
    float massScaleKoeff = mass / previousMass;
    ixyz *= massScaleKoeff;
    NewtonBodySetMassMatrix(m_body, mass, ixyz.x, ixyz.y, ixyz.z);
}

void PhysBody::setPos(vec3d pos) {
    auto mat = glm::translate(Identity, pos);
    NewtonBodySetMatrix(m_body, value_ptr(mat));
}

void PhysBody::setMatrix(mat4d mat) { NewtonBodySetMatrix(m_body, value_ptr(mat)); }

void PhysBody::setVelocity(vec3d vel) { NewtonBodySetVelocity(m_body, value_ptr(vel)); }

void PhysBody::addForce(vec3d force) { data.force += force; }

void PhysBody::addTorque(vec3d torque) { data.torque += torque; }

void PhysBody::setOmega(vec3d vel) { NewtonBodySetOmega(m_body, value_ptr(vel)); }

vec3d PhysBody::getPos() {
    vec3d position;
    NewtonBodyGetPosition(m_body, value_ptr(position));
    return position;
}

mat4d PhysBody::getMatrix() {
    mat4d matrix;
    NewtonBodyGetMatrix(m_body, value_ptr(matrix));
    return matrix;
}

vec3d PhysBody::getVelocity() {
    vec3d velocity;
    NewtonBodyGetVelocity(m_body, value_ptr(velocity));
    return velocity;
}

vec3d PhysBody::getOmega() {
    vec3d omega;
    NewtonBodyGetOmega(m_body, value_ptr(omega));
    return omega;
}

PhysBody::~PhysBody() {
    if (m_body) {
        NewtonDestroyBody(m_body);
        //DLOG("Body destroyed");
    }
}

// JOINTS
void PhysBody::setBallJoint(const PhysBody &body, vec3d pos) {
    NewtonConstraintCreateBall(NewtonBodyGetWorld(m_body), value_ptr(pos), body.m_body, m_body);
}

static uint sliderCallback(const NewtonJoint *const slider, NewtonHingeSliderUpdateDesc *const desc) {

    auto body0 = NewtonJointGetBody0(slider);
    auto body1 = NewtonJointGetBody1(slider);

    vec3d pos0, pos1;
    NewtonBodyGetPosition(body0, value_ptr(pos0));
    NewtonBodyGetPosition(body1, value_ptr(pos1));

    auto posOffset = pos1 - pos0;

    auto dist = NewtonSliderGetJointPosit(slider);
    auto vel = NewtonSliderGetJointVeloc(slider);

    double length = 6.0;
    double kSpring = 100.0;
    double kv = 10.0;
    auto restoringForce = normalize(posOffset) * (kSpring * (length - abs(dist)) - kv * vel);

    auto oppositeRestoringForce = -restoringForce;
    NewtonBodyAddForce(body0, value_ptr(oppositeRestoringForce));
    NewtonBodyAddForce(body1, value_ptr(restoringForce));
    return 0;
}

void PhysBody::setSlideJoint(const PhysBody &body, vec3d pos, vec3d dir) {
    auto joint = NewtonConstraintCreateSlider(NewtonBodyGetWorld(m_body), value_ptr(pos), value_ptr(dir), body.m_body, m_body);
    NewtonSliderSetUserCallback(joint, sliderCallback);
}

static uint corkscrewCallback(const NewtonJoint *const joint, NewtonHingeSliderUpdateDesc *const desc) {
    auto body0 = NewtonJointGetBody0(joint);
    auto body1 = NewtonJointGetBody1(joint);

    vec3d pos0, pos1;
    NewtonBodyGetPosition(body0, value_ptr(pos0));
    NewtonBodyGetPosition(body1, value_ptr(pos1));

    auto posOffset = pos1 - pos0;
    auto dist = NewtonCorkscrewGetJointPosit(joint);
    auto vel = NewtonCorkscrewGetJointVeloc(joint);

    //auto angle = NewtonCorkscrewGetJointAngle(joint);
    //auto omega = NewtonCorkscrewGetJointOmega(joint);

    double length = 6.0;
    double kSpring = 10.0;
    double kv = 1.0;
    auto restoringForce = normalize(posOffset) * (kSpring * (length - abs(dist)) - kv * vel);

    auto oppositeRestoringForce = -restoringForce;
    NewtonBodyAddForce(body0, value_ptr(oppositeRestoringForce));
    NewtonBodyAddForce(body1, value_ptr(restoringForce));
    return 0;
}

void PhysBody::setCorkscrewJoint(const PhysBody &body, vec3d pos, vec3d dir) {
    auto joint = NewtonConstraintCreateCorkscrew(NewtonBodyGetWorld(m_body), value_ptr(pos), value_ptr(dir), body.m_body, m_body);
    NewtonCorkscrewSetUserCallback(joint, corkscrewCallback);
}

void PhysBody::setHingeJoint(const PhysBody &body, vec3d pos, vec3d dir) {
    NewtonConstraintCreateHinge(NewtonBodyGetWorld(m_body), value_ptr(pos), value_ptr(dir), body.m_body, m_body);
}

void PhysBody::setUniversalJoint(const PhysBody &body, vec3d pivot, vec3d pin0, vec3d pin1) {
    NewtonConstraintCreateUniversal(NewtonBodyGetWorld(m_body), value_ptr(pivot), value_ptr(pin0), value_ptr(pin1), body.m_body, m_body);
}

void bilateralCallback(const NewtonJoint *const joint, dFloat timestep, int threadIndex) {

    //int solverModel = NewtonUserJointGetSolverModel(joint);
    //  NewtonUserJointSetSolverModel(joint, 1);
    //  NewtonUserJointSetFeedbackCollectorCallback (joint, NewtonUserBilateralCallback getFeedback);

    //  NewtonUserJointAddAngularRow(joint, dFloat relativeAngle, const dFloat *const dir);
    //  NewtonUserJointAddGeneralRow(joint, const dFloat *const jacobian0, const dFloat *const jacobian1);
    //  NewtonUserJointSetRowMinimumFriction(joint, dFloat friction);
    //  NewtonUserJointSetRowMaximumFriction(joint, dFloat friction);
    //double rowZeroAcceleeation = NewtonUserJointCalculateRowZeroAccelaration(joint);
    //double rowAcceleration = NewtonUserJointGetRowAcceleration(joint);

    //  NewtonUserJointSetRowAsInverseDynamics(joint);
    //  NewtonUserJointSetRowAcceleration (joint, 3.0);
    //  NewtonUserJointSetRowSpringDamperAcceleration (joint, dFloat rowStiffness, dFloat spring, dFloat damper);
    //NewtonUserJointSetRowStiffness(joint, 3.5);
    //int jointRowsCount = NewtonUserJoinRowsCount(joint);
    //NewtonUserJointGetGeneralRow(joint, int index, dFloat *const jacobian0, dFloat *const jacobian1);
    //double rowForce = NewtonUserJointGetRowForce(joint, 0 /*int row*/);
    //int submitImmediateMode = NewtonUserJointSubmitImmediateModeConstraint(joint, NewtonImmediateModeConstraint *const descriptor, dFloat timestep);

    auto body0 = NewtonJointGetBody0(joint);
    auto body1 = NewtonJointGetBody1(joint);
    vec3d p0{0, 0, 0}, p1;
    //NewtonBodyGetPosition(body0, value_ptr(p0));
    NewtonBodyGetPosition(body1, value_ptr(p1));
    vec3d pMid{4.0, 0.0, 4.0};
    vec3d vec{1.0, 0.0, 0.0};
    vec = normalize(vec);
    NewtonUserJointAddLinearRow(joint, value_ptr(p0), value_ptr(p1), value_ptr(vec));
}

void PhysBody::setUserJoint(const PhysBody &body) {
    auto joint = NewtonConstraintCreateUserJoint(NewtonBodyGetWorld(m_body), 6, bilateralCallback, body.m_body, m_body);
}
