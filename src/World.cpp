#include "../include/World.h"
#include "../include/Collisions.h"
#include <iostream>
#include <algorithm>
#include <list>


namespace pheng {

    pheng::World::World() {
        dt = 0;
    }

    World::World(float dt) {
        this->dt = dt;
    }

    pheng::World::~World() {
    }

    void World::addObject(Object* obj) {
        this->worldObjects.push_back(obj);
        n++;
    }

    void World::addObjects(std::vector<Object*> objects ) {
        for (auto &obj : objects){
            addObject(obj);
            n++;
        }
    }

    void World::removeObject(Object *obj) {
        if(n > 0){
            auto itr = std::find(worldObjects.begin(), worldObjects.end(), obj);
            if (itr == worldObjects.end()) return; // This means there's no such object in the world
            worldObjects.erase(itr);
        } else {
            printf("\n No objects in the World");
        }
    }

    void World::removeAll() {
        for(auto &obj: worldObjects){
            worldObjects.pop_back();
            delete obj;
        }
    }

    void World::step(double dt) {
        if(!isPaused) {
            for (Object* obj: worldObjects){
                if (obj->mobility == FREE){
                    obj->Force += (this->gravity * obj->Mass);
                    obj->Velocity += (obj->Force / obj->Mass) * dt;
                    obj->Position += (obj->Velocity * dt);

                    checkConstraintsCollision(obj);

                    obj->Force = {0.f, 0.f}; // Reinitializing the force applied to the Object
                    obj->applyChange();
                }
            }
        }
    }

    void World::detectCollisions() {
        //std::list<Object*> intersections;
        for (auto &obj: worldObjects) {
            for (auto &other: worldObjects) {
                if (obj != other) {
                    pheng::Collisions::circleToCircle(dynamic_cast<Circle*>(obj), dynamic_cast<Circle*>(other));
                }
            }
        }
    }

    void World::setGravity(vector2 value) {
        this->gravity = value;
    }

    void World::setDt(float dt) {
        this->dt = dt;
    }

    std::vector<Object*> World::getWorldObjects() {
        return this->worldObjects;
    }

    void World::setPaused() {
        this->isPaused = !isPaused;
    }

    void World::setConstraints(float x, float y) {
        this->constraints[0] = x;
        this->constraints[1] = y;
    }

    bool World::checkConstraintsCollision(Object* obj) {
        obj->constraintsCollision(constraints);
    }

    void World::spawnCircle(int x, int y) {
        auto* c = new pheng::Circle(50, {static_cast<double>(x),
                                         static_cast<double>(-y)}, 5);
        c->setVelocity({static_cast<float>(rand() % 10), static_cast<float>(rand() % 10)});

        //For Verlet's method
        c->Old_Position = c->Position;

        addObject(c);
    }



    // Verlet integration methods
    void World::update(float dt) {
        applyGravity();
        updatePositions(dt);
    }

    void World::updatePositions(float dt){
        for(auto &obj: worldObjects){
            obj->updatePositionVerlet(dt);
            obj->applyChange();
        }
    }

    void World::applyGravity() {
        for(auto &obj: worldObjects){
            obj->accelerate(gravity);
        }
    }
} // pheng


