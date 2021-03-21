#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool validPos(double x, double y);

    // If actor a overlaps some live actor that is affected by a holy water
    // projectile, inflict a holy water spray on that actor and return true;
    // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor* a);

    // Add an actor to the world.
    void addActor(Actor* a);

    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;

    // If actor a overlaps this world's GhostRacer, return a pointer to the
    // GhostRacer; otherwise, return nullptr
    //GhostRacer* getOverlappingGhostRacer(Actor* a) const;

    bool checkActorsInLane(Actor* a) const;

    GhostRacer* getRacer() { return racer; }
    Actor* getLastActor() { return actors.back(); }

    int getBonus() { return levelBonus; }
    void decBonus() { if(levelBonus >= 0) { --levelBonus; } }


private:
    int levelBonus = 5000;
    GhostRacer* racer;
    vector<Actor*> actors;
};

#endif // STUDENTWORLD_H_
