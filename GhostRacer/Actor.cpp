#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


void GhostRacer::doSomething() {

	if (getHP() <= 0) {
		getWorld()->decLives();
		dead();
	}

	if (getX() <= (ROAD_CENTER - (ROAD_WIDTH / 2.0))) {
		if (getDirection() > 90) {
			decHP(10);
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}

	if (getX() >= (ROAD_CENTER + (ROAD_WIDTH / 2.0))) {
		if (getDirection() < 90) {
			decHP(10);
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}

	double max_shift_per_tick = 4.0;
	int key;
	int direction = getDirection();
	double delta_x = cos(direction) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();

	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_SPACE:
			if (getNumSprays() > 0) {
				getWorld()->addActor(new Spray(getWorld(), getX(), sin(direction) + SPRITE_HEIGHT, direction));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				decSprays();
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) {
				setDirection(getDirection() + 8);
			}
			go(cur_x + delta_x, cur_y);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) {
				setDirection(getDirection() - 8);
			}
			go(cur_x + delta_x, cur_y);
			break;
		case KEY_PRESS_UP:
			if (getVert() < 5) {
				addSpeed(5);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getVert() > -1) {
				decSpeed(1);
			}
			break;
		default:
			break;
		}
	}
}

void GhostRacer::go(double x, double y) {
	if (getWorld()->validPos(x, y)) moveTo(x, y);
}

void GhostRacer::spin() {
	int randNum = rand();

	if (getDirection() >= 60 && getDirection() <= 120) {
		if (randNum % 2 == 0) {
			setDirection(getDirection() + randInt(5, 20));
		}
		else {
			setDirection(getDirection() - randInt(5, 20));
		}
	}
}

void BorderLine::doSomething() {
	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHori();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}	

}

void Pedestrian::doSomething() {
	if (isAlive() == false) {
		return;
	}

	if (getWorld()->overlaps(this, getWorld()->getRacer())) {
		getWorld()->decLives();
		getWorld()->getRacer()->dead();
		return;
	}

	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	decrementMPD();

	if (getMPD() > 0) {
		return;
	}
	else {
		double newSpeed = 0;
		do {
			setNewHSpeed(randInt(-3, 3));
			newSpeed = getHorizSpeed();
		} while (newSpeed == 0);
		setMPD(randInt(4, 32));
		if (newSpeed < 0) {
			setDirection(180);
		}
		else {
			setDirection(0);
		}
	}
}

bool Pedestrian::beSprayedIfAppropriate() {
	invertHSpeed();

	if (getDirection() == 0) {
		setDirection(getDirection() + 180);
	}
	else {
		setDirection(getDirection() - 180);
	}
	getWorld()->playSound(SOUND_PED_HURT);

	return true;
}


void ZombiePed::doSomething() {
	if (isAlive() == false) {
		return;
	}

	if (getWorld()->overlaps(this, getWorld()->getRacer())) {
		getWorld()->getRacer()->decHP(5);
		this->decHP(2);
		if (this->getHP() <= 0) {
			dead();
			getWorld()->playSound(SOUND_PED_DIE);
			getWorld()->increaseScore(150);
		}
		else {
			getWorld()->playSound(SOUND_PED_HURT);
		}
		return;
	}

	if (std::abs(this->getX() - getWorld()->getRacer()->getX()) < 30 && this->getY() > getWorld()->getRacer()->getY()) {
		setDirection(270);
		if (getX() < getWorld()->getRacer()->getX()) {
			setNewHSpeed(1);
		}
		else if (getX() > getWorld()->getRacer()->getX()) {
			setNewHSpeed(-1);
		}
		else {
			setNewHSpeed(0);
		}
		decrementTicks();
		if (getTicks() <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			resetTicks();
		}
	}

	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (getMPD() > 0) {
		decrementMPD();
		return;
	}
	else {
		double newSpeed = 0;
		do {
			setNewHSpeed(randInt(-3, 3));
			newSpeed = getHorizSpeed();
		} while (newSpeed == 0);
		setMPD(randInt(4, 32));
		if (newSpeed < 0) {
			setDirection(180);
		}
		else {
			setDirection(0);
		}
	}
}

bool ZombiePed::beSprayedIfAppropriate() {
	decHP(1);

	if (getHP() <= 0) {
		dead();
		getWorld()->playSound(SOUND_PED_DIE);
		if (!getWorld()->overlaps(this, getWorld()->getRacer())) {
			int num = randInt(1, 5);
			if (num == 1) {
				getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
			}
		}
		getWorld()->increaseScore(150);
	}
	else {
		getWorld()->playSound(SOUND_PED_HURT);
	}

	return true;
}

void ZombieCab::doSomething() {
	if (isAlive() == false) {
		return;
	}

	if (getWorld()->overlaps(this, getWorld()->getRacer())) {
		if (getDamagedStatus() == true) {
			double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
			double horiz_speed = this->getHSpeed();
			double new_y = getY() + vert_speed;
			double new_x = getX() + horiz_speed;

			moveTo(new_x, new_y);

			if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
				this->dead();
				return;
			}

			if (this->getVert() > getWorld()->getRacer()->getVert() && getWorld()->checkActorsInLane(this)) {
				this->decSpeed(0.5);
				return;
			}

			if (this->getVert() <= getWorld()->getRacer()->getVert() && getWorld()->checkActorsInLane(this)) {
				this->addSpeed(0.5);
				return;
			}

			decrementMPD();

			if (getMPD() > 0) {
				return;
			}
			else {
				setMPD(randInt(4, 32));
				setVSpeed(getVert() + randInt(-2, 2));
			}
		}
		else {
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getRacer()->decHP(20);
			if (getX() <= getWorld()->getRacer()->getX()) {
				setHorizontalSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else {
				setHorizontalSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			setStatus();
		}
	}
}

bool ZombieCab::beSprayedIfAppropriate() {
	decHP(1);

	if (getHP() <= 0) {
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		int num = randInt(1, 5);
		if (num == 1) {
			getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
		}
		getWorld()->increaseScore(200);
		dead();
		return true;
	}
	else {
		getWorld()->playSound(SOUND_VEHICLE_HURT);
		return true;
	}

	return false;
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate() {
	if (isSprayable()) {
		dead();
	}
	return false;
}

bool GhostRacerActivatedObject::overlapping() {
	double delta_x = std::abs(this->getX() - getWorld()->getRacer()->getX());
	double delta_y = std::abs(this->getY() - getWorld()->getRacer()->getY());
	double radiusSum = this->getRadius() + getWorld()->getRacer()->getRadius();

	if (delta_x < radiusSum * 0.25 && delta_y < radiusSum * 0.6) {
		return true;
	}

	return false;
}

void OilSlick::doSomething() {
	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHori();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (overlapping()) {
		doActivity(getWorld()->getRacer());
	}
}

void OilSlick::doActivity(GhostRacer* gr) {
	getWorld()->playSound(SOUND_OIL_SLICK);
	gr->spin();
}


void HealingGoodie::doSomething() {
	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHori();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (overlapping()) {
		doActivity(getWorld()->getRacer());
	}
}

void HealingGoodie::doActivity(GhostRacer* gr) {
	gr->getHP(10);
	this->dead();
	getWorld()->playSound(IID_HEAL_GOODIE);
	getWorld()->increaseScore(getScoreIncrease());
}


void HolyWaterGoodie::doSomething() {
	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHori();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (overlapping()) {
		doActivity(getWorld()->getRacer());
	}
}

void HolyWaterGoodie::doActivity(GhostRacer* gr) {
	gr->increaseSprays(10);
	this->dead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(getScoreIncrease());
}


void SoulGoodie::doSomething() {
	double vert_speed = this->getVert() - getWorld()->getRacer()->getVert();
	double horiz_speed = this->getHori();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (overlapping()) {
		doActivity(getWorld()->getRacer());
	}
	
	setDirection(getDirection() + 10);

}

void SoulGoodie::doActivity(GhostRacer* gr) {
	gr->addSoul(1);
	this->dead();
	getWorld()->playSound(SOUND_GOT_SOUL);
	getWorld()->increaseScore(getScoreIncrease());
}


void Spray::doSomething() {
	if (isAlive() == false) {
		return;
	}
	getWorld()->sprayFirstAppropriateActor(this);

	moveForward(SPRITE_HEIGHT);
	remainingDistance(SPRITE_HEIGHT);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		this->dead();
		return;
	}

	if (getMaxDistance() <= 0) {
		this->dead();
	}
}

