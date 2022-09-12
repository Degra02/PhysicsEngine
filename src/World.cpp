#include "../include/World.h"
#include "../include/Collisions.h"
#include <iostream>
#include <algorithm>
#include <list>
#include <cmath>


namespace pheng {

    pheng::World::World(const sf::Font& font) {
        dt = 0;

        Legend.setFont(font);
        Legend.setCharacterSize(24);
        Legend.setFillColor(sf::Color::White);
        Legend.setPosition({50, 50});

        objLegend.setFont(font);
        objLegend.setCharacterSize(24);
        objLegend.setFillColor(sf::Color::White);
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
            n--;
        } else {
            printf("\n No objects in the World");
        }
    }

    void World::removeAll() {
        for(uint32_t i(0); i < n; ++i){
            Object* obj = worldObjects[i];
            worldObjects.pop_back();
            delete obj;
        }
        n = 0; total_energy = 0;
        updateLegend();
    }

    void World::step(float dt, float r_c = 1) {
        if (!isPaused && n > 0) {
            total_energy = 0;
            n_outside = 0;
            for (uint32_t i(0); i < n; ++i) {
                if (worldObjects[i]->mobility == FREE){
                    worldObjects[i]->Force += (this->gravity * worldObjects[i]->Mass);
                    worldObjects[i]->Velocity += (worldObjects[i]->Force / worldObjects[i]->Mass) * dt;
                    worldObjects[i]->Position += (worldObjects[i]->Velocity * dt);
                    checkConstraintsCollision(worldObjects[i], r_c);

                    if (worldObjects[i]->Position.getX() < 0 || worldObjects[i]->Position.getX() > window_constraints[0] ||
                        worldObjects[i]->Position.getY() < 0 || worldObjects[i]->Position.getY() > window_constraints[1]) {
                        n_outside++;
                    }

                    worldObjects[i]->Force = {0.f, 0.f}; // Reinitializing the force applied to the Object
                    worldObjects[i]->applyChange();

                    updateEnergy(worldObjects[i]);
                    worldObjects[i]->changeColorVelocity();
                }
            }
            SweepAndPrune::getPossibleCollisions(worldObjects, r_c);
            updateLegend();
        }
    }

    void World::updateEnergy(Object* obj) {
        total_energy += obj->calculateEnergy(window_constraints[1], gravity.getY() / 100.f);
    }

    void World::updateLegend() {
        std::stringstream s;
        s << "Energy: " << std::setprecision(3) << total_energy << std::endl \
            << "No: " << n << std::endl \
            << "No outside: " << n_outside << std::endl;
        Legend.setString(s.str());
    }

    void World::updateObjLegend(Object *obj, vector2 offsetCenter) {
        std::stringstream s;
        s << "Velocity: " << std::setprecision(3) << obj->Velocity.norm() / 100 << "m/s" << std::endl;
        objLegend.setPosition(offsetCenter.getX() + 50, offsetCenter.getY());
        objLegend.setString(s.str());
    }

    [[deprecated("Replaced by Sweep & Prune")]]
    void World::detectCollisionsBruteForce(uint32_t i, Object* obj, float r_c) {
        for (uint32_t k(i+1); k < n; ++k){
            pheng::Collisions::objToObj(obj, worldObjects[k], r_c);
        }
    }

    void World::setGravity(vector2 value) {
        this->gravity = value;
    }

    void World::setDt(float dt) {
        this->dt = dt;
    }


    std::vector<Object*> World::getWorldObjects() const {
        return this->worldObjects;
    }

    void World::setPaused() {
        this->isPaused = !isPaused;
    }

    void World::setConstraints(float x, float y) {
        this->window_constraints[0] = x;
        this->window_constraints[1] = y;
    }

    bool World::checkConstraintsCollision(Object* obj, float r_f) {
        obj->constraintsCollision(window_constraints, r_f);
    }

    void World::spawnCircle(int x, int y) {
        float r = rand() % (50 - 10 + 1) + 10;

        auto* c = new pheng::Circle(r, {static_cast<float>(x), static_cast<float>(y)}, r);
        //c->setVelocity({static_cast<float>(rand() % 100), static_cast<float>(rand() % 100)});

        //For Verlet's method
        c->Old_Position = c->Position;

        addObject(c);
    }

    void World::addSpawner(ObjectSpawner *spawner) {
        spawners.push_back(spawner);
    }


    void World::removeSpawner(ObjectSpawner* spawner) {
        auto itr = std::find(spawners.begin(), spawners.end(), spawner);
        if (itr == spawners.end()) return;
        spawners.erase(itr);
    }

    void World::spawnObjectsSpawner() {
        if (!isPaused) {
            for (auto &spawner: spawners) {
                if (spawner->isSpawning) {
                    Object* obj = spawner->spawnObject();
                    if (obj != nullptr)
                        addObject(obj);
                }
            }
        }
    }

    void World::toggleSpawners() {
        for (auto &spawner: spawners) {
            spawner->toggleSpawning();
        }
    }

    void World::addCannon(Cannon* cannon) {
        cannons.push_back(cannon);
    }

    void World::removeCannon(Cannon* cannon) {
        auto itr = std::find(cannons.begin(), cannons.end(), cannon);
        if (itr == cannons.end()) return;
        cannons.erase(itr);
    }

    void World::toggleCannons() {
        if (!isPaused) {
            for (auto &cannon: cannons){
                addObject(cannon->shootObject());
            }
        }
    }

    //Drawing objects
    void World::drawObjects(sf::RenderWindow *window) {
        for (auto &obj: worldObjects) {
            window->draw(obj->getDrawable());
        }
        window->draw(Legend);
        window->draw(objLegend);
    }
} // pheng


