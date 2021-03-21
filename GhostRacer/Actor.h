#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <random>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject {

public:

	Actor(StudentWorld* world, int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool avoid) : GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), canAvoid(avoid) {};

	virtual void doSomething() = 0;
	virtual bool isAlive() = 0;
	virtual void dead() = 0;
	StudentWorld* getWorld() { return m_world; };

	// If this actor is affected by holy water projectiles, then inflict that
	// affect on it and return true; otherwise, return false.
	virtual bool beSprayedIfAppropriate() = 0;

	// Does this object affect zombie cab placement and speed?
	virtual bool isCollisionAvoidanceWorthy() const { return canAvoid; }

	// Adjust the x coordinate by dx to move to a position with a y coordinate
	// determined by this actor's vertical speed relative to GhostRacser's
	// vertical speed.  Return true if the new position is within the view;
	// otherwise, return false, with the actor dead.
	//virtual bool moveRelativeToGhostRacerVerticalSpeed(double dx);

private:
	StudentWorld* m_world;
	bool canAvoid;
};

class LivingThing : public Actor {

public: 
	LivingThing(StudentWorld* world, int imageID, double startX, double startY, int dir, double size, unsigned int depth, int _health, double _vSpeed, bool avoid, bool _alive) : Actor(world, imageID, startX, startY, dir, size, depth, avoid), alive(_alive), health(_health), verticalSpeed(_vSpeed) {};

	virtual void dead() { alive = false; return; }
	virtual bool isAlive() { return alive; }
	double getVert() { return verticalSpeed; }

	void setVSpeed(double amt) { verticalSpeed = amt; }

	/*void addSpeed(int i) {
		verticalSpeed += i;
	}

	void decSpeed(int i) {
		verticalSpeed -= i;
	}*/

	void addSpeed(double i) {
		verticalSpeed += i;
	}

	void decSpeed(double i) {
		verticalSpeed -= i;
	}

	// Get hit points.
	int getHP() {
		return health;
	}

	// Increase hit points by hp.
	void getHP(int hp) {
		health += hp;
	}

	void decHP(int hp) {
		health -= hp;
	}

	// Do what the spec says happens when hp units of damage is inflicted.
	// Return true if this agent dies as a result, otherwise false.
	//virtual bool takeDamageAndPossiblyDie(int hp) = 0;

	// What sound should play when this agent is damaged but does not die?
	//virtual int soundWhenHurt() = 0;

	// What sound should play when this agent is damaged and dies?
	//virtual int soundWhenDie() = 0;

private:
	bool alive;
	int health;
	double verticalSpeed;
};

class BorderLine : public LivingThing {

public:
	BorderLine(StudentWorld* world, int imageID, double x, double y) : LivingThing(world, imageID, x, y, 0, 2.0, 2, 0, -4, false, true), horizontalSpeed(0) {};

	virtual void doSomething();
	virtual bool beSprayedIfAppropriate() { return false; }
	double getHori() { return horizontalSpeed; }

private:
	double horizontalSpeed;
};

class GhostRacer : public LivingThing {

public:
	GhostRacer(StudentWorld* world) : LivingThing(world, IID_GHOST_RACER, 128.0, 32.0, 90, 4.0, 0, 100, 0, true, true), holyWaterAmmo(10), soulsSaved(0) {};

	virtual void doSomething();

	virtual bool beSprayedIfAppropriate() { return false; }

	//virtual int soundWhenDie() const;

	// How many holy water projectiles does the object have?
	int getNumSprays() const { return holyWaterAmmo; }

	void decSprays() { --holyWaterAmmo; }

	int getSoulsSaved() const { return soulsSaved; }

	void addSoul(int amt) { soulsSaved += amt; }

	// Increase the number of holy water projectiles the object has.
	void increaseSprays(int amt) { holyWaterAmmo += amt; }

	// Spin as a result of hitting an oil slick.
	void spin();

private:
	int holyWaterAmmo;
	int soulsSaved;
	void go(double x, double y);
};

class Walker : public LivingThing {

public:
	Walker(StudentWorld* world, int imageID, double _startX, double _startY, double size, double _hSpeed, int _MPD) : LivingThing(world, imageID, _startX, _startY, 0, size, 0, 2, -4, true, true), horizontalSpeed(_hSpeed), movementPlanDistance(_MPD) {};

	//virtual int soundWhenHurt() const;
	//virtual int soundWhenDie() const;

	// Get the pedestrian's horizontal speed
	double getHorizSpeed() const { return horizontalSpeed; }

	void setNewHSpeed(double amt) { horizontalSpeed = amt; }

	void invertHSpeed() { horizontalSpeed += -1; }

	int getMPD() const { return movementPlanDistance; }

	void decrementMPD() { --movementPlanDistance; }

	void setMPD(int amt) { movementPlanDistance = amt; }

	// Set the pedestrian's horizontal speed
	void setHorizSpeed(double s) {
		horizontalSpeed += s;
	}

	// Move the pedestrian.  If the pedestrian doesn't go off screen and
	// should pick a new movement plan, pick a new plan.
	//void moveAndPossiblyPickPlan();

private:
	double horizontalSpeed;
	int movementPlanDistance;
};

class Pedestrian : public Walker {

public:
	Pedestrian(StudentWorld* world, double startX, double startY) : Walker(world, IID_HUMAN_PED, startX, startY, 2.0, 0, 0) {}

	virtual void doSomething();

	virtual bool beSprayedIfAppropriate();
	//virtual bool takeDamageAndPossiblyDie(int hp);

private:

};

class ZombiePed : public Walker {

public:
	ZombiePed(StudentWorld* world, double _startX, double _startY) : Walker(world, IID_ZOMBIE_PED, _startX, _startY, 3.0, 0, 0), ticks(0) {};

	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
	int getTicks() { return ticks; }
	void decrementTicks() { --ticks; }
	void resetTicks() { ticks = 20; }

private:
	int ticks;
};

class ZombieCab : public LivingThing {

public:
	ZombieCab(StudentWorld* world, double x, double y) : LivingThing(world, IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, 3.0, 0.0, true, true), planLength(0), horizontalSpeed(0.0), hasDamagedPlayer(false) {}
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
	bool getDamagedStatus() { return hasDamagedPlayer; }
	void setStatus() { hasDamagedPlayer = true; }
	void setHorizontalSpeed(double amt) { horizontalSpeed = amt; }
	double getHSpeed() { return horizontalSpeed; }
	int getMPD() { return planLength; }
	void setMPD(int amt) { planLength = amt; }
	void decrementMPD() { --planLength; }

private:
	int planLength;
	double horizontalSpeed;
	bool hasDamagedPlayer;
};

class GhostRacerActivatedObject : public Actor
{
public:
	GhostRacerActivatedObject(StudentWorld* world, int imageID, double x, double y, double size, int dir, unsigned int depth, bool avoid, double vertSpeed, double hSpeed) : Actor(world, imageID, x, y, dir, size, depth, avoid), alive(true), verticalSpeed(vertSpeed), horizontalSpeed(hSpeed) {}
	virtual bool beSprayedIfAppropriate();

	// Do the object's special activity (increase health, spin Ghostracer, etc.)
	virtual void doActivity(GhostRacer* gr) = 0;

	// Return the object's increase to the score when activated.
	virtual int getScoreIncrease() const = 0;

	// Return the sound to be played when the object is activated.
	//virtual int getSound() const;

	// Return whether the object dies after activation.
	virtual bool selfDestructs() const = 0;

	// Return whether the object is affected by a holy water projectile.
	virtual bool isSprayable() const = 0;

	virtual bool overlapping();

	virtual bool isAlive() { return alive; }
	virtual void dead() { alive = false; return; }

	double getVert() { return verticalSpeed; }
	double getHori() { return horizontalSpeed; }

private:
	bool alive;
	double verticalSpeed, horizontalSpeed;
};

class OilSlick : public GhostRacerActivatedObject
{
public:
	OilSlick(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, (double) randInt(2, 5), 0, 2, false, -4, 0) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const { return 0; };
	//virtual int getSound() const { return SOUND_OIL_SLICK; }
	virtual bool selfDestructs() const { return false; }
	virtual bool isSprayable() const { return false; }

private:

};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
	HealingGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1.0, 0, 2, false, 0, -4) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const { return 250; }
	virtual bool selfDestructs() const { return true; }
	virtual bool isSprayable() const { return true; }
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
	HolyWaterGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2.0, 90, 2, false, -4, 0) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const { return 50; }
	virtual bool selfDestructs() const { return true; }
	virtual bool isSprayable() const { return true; }
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
	SoulGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4.0, 0, 2, false, -4, 0) {}
	virtual void doSomething();
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const { return 100; }
	//virtual int getSound() const { return SOUND_GOT_SOUL; }
	virtual bool selfDestructs() const { return true; }
	virtual bool isSprayable() const { return false; }

private:

};

class Spray : public Actor
{
public:
	Spray(StudentWorld* sw, double x, double y, int dir) : Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, false), alive(true), maxTravelDistance(160) {}
	virtual void doSomething();
	virtual bool isAlive() { return alive; }
	void dead() { alive = false; return; }
	int getMaxDistance() const { return maxTravelDistance; }
	void remainingDistance(int amt) { maxTravelDistance -= amt; }
	virtual bool beSprayedIfAppropriate() { return false; }

private:
	bool alive;
	int maxTravelDistance;
};

#endif // ACTOR_H_
