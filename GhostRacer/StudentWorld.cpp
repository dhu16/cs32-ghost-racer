#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	racer = nullptr;
	actors.clear();
}

int StudentWorld::init()
{
	const double N = VIEW_HEIGHT / SPRITE_HEIGHT;
	const double M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
	const double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH / 2.0);
	const double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH / 2.0);

	racer = new GhostRacer(this);

	for (double i = 0; i < N; ++i) {
		actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
		actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
	}

	for (double j = 0; j < M; ++j) {
		actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + (ROAD_WIDTH/ 3.0), j * (4.0 * SPRITE_HEIGHT)));
		actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - (ROAD_WIDTH / 3.0), j * (4.0 * SPRITE_HEIGHT)));
	}

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	const int LEFT = ROAD_CENTER - (ROAD_WIDTH / 3);
	const int RIGHT = ROAD_CENTER + (ROAD_WIDTH / 3);
	const int MID = ROAD_CENTER;

	racer->doSomething();
	for (Actor* a : actors) {
		if (a->isAlive() == true) {
			a->doSomething();
			if (racer->isAlive() == false) {
				return GWSTATUS_PLAYER_DIED;
			} 
			else if (racer->getSoulsSaved() == (2 * getLevel() + 5)) {
				advanceToNextLevel();
				increaseScore(getBonus());
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	}

	decBonus();

	//Remove dead actors
	for (unsigned int i = 0; i < actors.size(); ++i) {
		if (actors[i]->isAlive() == false) {
			delete actors[i];
			actors[i] = actors[actors.size() - 1];
			actors.pop_back();
			--i;
		}
	}
	
	//Update borders
	int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
	double delta_y = new_border_y - getLastActor()->getY();

	if (delta_y >= SPRITE_HEIGHT) {
		actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - (ROAD_WIDTH / 2.0), new_border_y));
		actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + (ROAD_WIDTH / 2.0), new_border_y));
	}

	if (delta_y >= 4.0 * SPRITE_HEIGHT) {
		actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - (ROAD_WIDTH / 2.0) + (ROAD_WIDTH / 3.0), new_border_y));
		actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + (ROAD_WIDTH / 2.0) - (ROAD_WIDTH / 3.0), new_border_y));
	}

	//Spawn zombie cab
	int chanceCab = max(100 - getLevel() * 10, 20);
	int r1 = randInt(0, chanceCab - 1);
	bool botcollisionObjExists = false;
	bool existsGreaterThan = false;
	bool existsLessThan = false;
	bool topExists = false;
	bool viableLane = false;
	int cur_lane = 0;

	if (r1 == 0) {
		int start_Y = 0;
		double initial_vert = 0.0;
		vector<int> list = { LEFT, MID, RIGHT };

		for (int i = 0; i < 3; ++i) {
			int index = rand() % list.size();
			cur_lane = list[index];
			list.erase(list.begin() + index);

			for (Actor* a : actors) {
				if (a->isCollisionAvoidanceWorthy() == true && a->getY() > (VIEW_HEIGHT / 3)) {
					existsGreaterThan = true;
				}
				else if (a->isCollisionAvoidanceWorthy() == true && a->getY() <= (VIEW_HEIGHT / 3)) {
					botcollisionObjExists = true;
				}
			}

			if (botcollisionObjExists == false || existsGreaterThan == true) {
				start_Y = SPRITE_HEIGHT / 2;
				initial_vert = getRacer()->getVert() + randInt(2, 4);
				viableLane = true;
				break;
			}

			for (Actor* a : actors) {
				if (a->isCollisionAvoidanceWorthy() == true && a->getY() < ((VIEW_HEIGHT * 2) / 3)) {
					existsLessThan = true;
				}
				else if (a->isCollisionAvoidanceWorthy() == true && a->getY() >= ((VIEW_HEIGHT * 2) / 3)) {
					topExists = true;
				}
			}

			if (topExists == false || existsLessThan == true) {
				start_Y = ((VIEW_HEIGHT - SPRITE_HEIGHT) / 2);
				initial_vert = getRacer()->getVert() - randInt(2, 4);
				viableLane = true;
				break;
			}

			if (topExists == true && botcollisionObjExists == true) {
				continue;
			}
		}

		if (viableLane == true) {
			ZombieCab* cab = new ZombieCab(this, cur_lane, start_Y);
			cab->setVSpeed(initial_vert);
			addActor(cab);
		}
	}


	//Spawn oil slicks
	int chanceOilSlick = max(150 - getLevel() * 10, 40);
	int r2 = randInt(0, chanceOilSlick - 1);

	if (r2 == 0) {
		double rand_x = randInt(0, ROAD_WIDTH);
		addActor(new OilSlick(this, rand_x, VIEW_HEIGHT));
	}


	//Spawn humans
	int chanceHuman = max(200 - getLevel() * 10, 30);
	int r3 = randInt(0, chanceHuman - 1);

	if (r3 == 0) {
		double rand_x = randInt(0, VIEW_WIDTH);
		addActor(new Pedestrian(this, rand_x, VIEW_HEIGHT));
	}


	//Spawn zombies
	int chanceZombie = max(100 - getLevel() * 10, 20);
	int r4 = randInt(0, chanceZombie - 1);

	if (r4 == 0) {
		double rand_x = randInt(0, VIEW_WIDTH);
		addActor(new ZombiePed(this, rand_x, VIEW_HEIGHT));
	}


	//Spawn holy water
	int chanceWater = 100 + 10 * getLevel();
	int r5 = randInt(0, chanceWater - 1);

	if (r5 == 0) {
		double rand_x = randInt(0, ROAD_WIDTH);
		addActor(new HolyWaterGoodie(this, rand_x, VIEW_HEIGHT));
	}


	//Spawn lost souls
	int chanceSoul = 100;
	int r6 = randInt(0, chanceSoul - 1);

	if (r6 == 0) {
		double rand_x = randInt(0, ROAD_WIDTH);
		addActor(new SoulGoodie(this, rand_x, VIEW_HEIGHT));
	}

	//Update game status
	ostringstream oss;
	oss << "Score: " << getScore() << setw(7) << "Lvl: " << getLevel() << setw(14) << "Souls2Save: " << getRacer()->getSoulsSaved() << setw(9) << "Lives: " << getLives() << setw(10) << "Health: " << getRacer()->getHP() << setw(10) << "Sprays: " << getRacer()->getNumSprays() << setw(9) << "Bonus: " << getBonus() << endl;
	string s = oss.str();
	setGameStatText(s);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (Actor* a : actors) {
		delete a;
	}

	actors.clear();

	delete racer;
	racer = nullptr;
}

bool StudentWorld::validPos(double x, double y) 
{
	for (auto a : actors) {
		if (x + SPRITE_WIDTH - 1 > a->getX() && x < a->getX() + SPRITE_WIDTH - 1) {
			if (y + SPRITE_HEIGHT - 1 > a->getY() && y < a->getY() + SPRITE_HEIGHT - 1) {
				return false;
			}
		}
	}
	return true;
}

void StudentWorld::addActor(Actor* a) {
	actors.push_back(a);
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* a) {
	for (Actor* b : actors) {
		if (overlaps(a, b) && b->beSprayedIfAppropriate() == true) {
			a->dead();
			return true;
		}
	}
	return false;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const {
	double delta_x = std::abs(a1->getX() - a2->getX());
	double delta_y = std::abs(a1->getY() - a2->getY());
	double radiusSum = a1->getRadius() + a2->getRadius();

	if (delta_x < radiusSum * 0.25 && delta_y < radiusSum * 0.6) {
		return true;
	}

	return false;
}

bool StudentWorld::checkActorsInLane(Actor* a) const {
	for (Actor* b : actors) {
		if (b->isCollisionAvoidanceWorthy() == true && b->getY() > a->getY()) {
			if (abs(b->getY() - a->getY() < 96)) {
				return true;
			}
		}
		else if (b->isCollisionAvoidanceWorthy() == true && b->getY() < a->getY()) {
			if (abs(b->getY() - a->getY() < 96)) {
				return true;
			}
		}
	}

	return false;
}
