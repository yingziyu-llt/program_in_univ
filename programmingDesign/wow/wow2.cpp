/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <vector>

enum ColorType {
	red = 0,
	blue = 1
};
enum WarriorType {
	dragon = 0,
	ninja = 1,
	iceman = 2,
	lion = 3,
	wolf = 4
};
enum WeaponType {
	sword = 0,
	bomb = 1,
	arrow = 2
};
const char KINDNAMES[5][10]
	= { "dragon", "ninja", "iceman", "lion", "wolf" };
const char WEAPONS[3][10] = { "sword", "bomb", "arrow" };
const int RED_ORDER[5] = { 2, 3, 4, 1, 0 }, BLUE_ORDER[] = { 3, 0, 1, 2, 4 };
int Time;
/*****************Basic Classes**********************/
class Warrior;
class Weapon;
class Headquarter;
/*************** headquarter ********************/
class Headquarter {
public:
	static int Cost[10];
	int Order[5];
	int Count[10];
	int Health;
	int currentPosition;
	bool ableToSummon;
	int totalCount;
	ColorType color;
	std::vector<Warrior*> warriors;
	void init()
	{
		this->warriors.clear();
		for (int i = 0; i <= 9; i++)
			Cost[i] = 0, Count[i] = 0;
		Health = 0;
		ableToSummon = 1;
		currentPosition = -1;
		totalCount = 0;
	}
	bool available(Warrior* x);
	void Summon();
} Red, Blue;
int Headquarter::Cost[10] = { 0 };

/********************************/

/*************** weapon ********************/
class Weapon {
protected:
	Weapon() { }

public:
	char name[20];
	int attack;
	virtual ~Weapon() {};
	virtual char* getName() = 0;
};
class Sword : public Weapon {
	const char name[6] = "sword";
	char* getName() { return (char*)name; }
};
class Bomb : public Weapon {
	const char name[6] = "bomb";
	char* getName() { return (char*)name; }
};
class Arrow : public Weapon {
	const char name[6] = "arrow";
	char* getName() { return (char*)name; }
};
/****************************/

/*************** warriors ********************/
class Warrior {

public:
	int Health;

protected:
	Warrior(int health)
	{
		this->Health = health;
	}

public:
	void SummonPrint(int col, Headquarter* H, char* name, WarriorType wa)
	{
		std::cout << std::setw(3) << std::setfill('0') << Time
				  << " " << (col == 0 ? "red" : "blue") << " " << name << " " << H->totalCount
				  << " born with strength " << this->Health << "," << H->Count[wa]
				  << " " << name << " "
				  << "in " << (col == 0 ? "red" : "blue") << " headquarter" << std::endl;
	}
	virtual ~Warrior() { }
};
class WarriorWithWeapon : public Warrior {
public:
	Weapon* Weapons[2];

protected:
	WarriorWithWeapon(int health, int count, int type = 1)
		: Warrior(health)
	{
		if (type == 1)
			newWeapon(count, Weapons[0]);
		else
			newWeapon(count, Weapons[0], Weapons[1]);
	}

public:
	void SummonPrint(int col, Headquarter* H, char* name, WarriorType wa)
	{
		((Warrior*)this)->SummonPrint(col, H, name, wa);
		std::cout << "It has a " << this->Weapons[0]->getName();
	}
	void newWeapon(int count, Weapon*& p1);
	void newWeapon(int count, Weapon*& p1, Weapon*& p2);
	~WarriorWithWeapon()
	{
		delete Weapons[0];
	}
};

class Iceman : public WarriorWithWeapon {
public:
	Iceman(ColorType col)
		: WarriorWithWeapon(Headquarter::Cost[(int)iceman], col == red ? Red.totalCount : Blue.totalCount)
	{
	}
	void SummonPrint(int col, Headquarter* H)
	{
		((WarriorWithWeapon*)this)->SummonPrint(col, H, "iceman", iceman);
		std::cout << std::endl;
	}
};
class Dragon : WarriorWithWeapon {
public:
	double morale;
	Dragon(ColorType col)
		: WarriorWithWeapon(Headquarter::Cost[dragon], col == red ? Red.totalCount : Blue.totalCount)
	{
		switch (col) {
		case red:
			morale = (double)Red.Health / (double)Headquarter::Cost[dragon];
			break;
		case blue:
			morale = (double)Blue.Health / (double)Headquarter::Cost[dragon];
			break;
		default:
			break;
		}
	}
	void SummonPrint(int col, Headquarter* H)
	{
		((WarriorWithWeapon*)this)->SummonPrint(col, H, "dragon", dragon);
		std::cout << ",and it's morale is " << std::fixed << std::setprecision(2) << morale << std::endl;
	}
};
class Ninja : public WarriorWithWeapon {
public:
	Ninja(int col)
		: WarriorWithWeapon(Headquarter::Cost[ninja], col == red ? Red.totalCount : Blue.totalCount, 2)
	{
	}
	void SummonPrint(int col, Headquarter* H)
	{
		((WarriorWithWeapon*)this)->SummonPrint(col, H, "ninja", ninja);
		std::cout << " and a " << this->Weapons[1]->getName() << std::endl;
	}
	~Ninja()
	{
		delete this->Weapons[1];
	}
};
class Lion : public Warrior {
public:
	int loyalty;
	Lion(int col)
		: Warrior(col == red ? Red.Cost[lion] : Blue.Cost[lion])
	{
		switch (col) {
		case red:
			loyalty = Red.Health;
			break;
		case blue:
			loyalty = Blue.Health;
			break;
		default:
			break;
		}
	}
	void SummonPrint(int col, Headquarter* H)
	{
		((Warrior*)this)->SummonPrint(col, H, "lion", lion);
		std::cout << "It's loyalty is " << this->loyalty << std::endl;
	}
};
class Wolf : public Warrior {
public:
	Wolf(int col)
		: Warrior(col == red ? Red.Cost[wolf] : Blue.Cost[wolf])
	{
	}
	void SummonPrint(int col, Headquarter* H)
	{
		((Warrior*)this)->SummonPrint(col, H, "wolf", wolf);
	}
};

/*********************/
void WarriorWithWeapon::newWeapon(int count, Weapon*& p1)
{
	switch (count % 3) {
	case sword:
		p1 = (Weapon*)new Sword;
		break;
	case arrow:
		p1 = (Weapon*)new Arrow;
		break;
	case bomb:
		p1 = (Weapon*)new Bomb;
		break;
	}
}
void WarriorWithWeapon::newWeapon(int count, Weapon*& p1, Weapon*& p2)
{
	this->newWeapon(count, p1);
	this->newWeapon(count + 1, p2);
}
bool Headquarter::available(Warrior* x)
{
	if (x->Health <= this->Health)
		return true;
	else
		return false;
}
void Headquarter::Summon()
{
	if (!ableToSummon)
		return;
	Warrior *piceman, *plion, *pwolf, *pninja, *pdragon;
	piceman = (Warrior*)new Iceman(this->color);
	pwolf = (Warrior*)new Wolf(this->color);
	pninja = (Warrior*)new Ninja(this->color);
	pdragon = (Warrior*)new Dragon(this->color);
	plion = (Warrior*)new Lion(this->color);
	int try_cnt;
	bool flag = false;
	for (try_cnt = 1; try_cnt <= 5; try_cnt++) {
		currentPosition++;
		currentPosition %= 5;
		switch (Order[currentPosition]) {
		case iceman:
			if (available(piceman)) {
				totalCount++;
				Health -= Cost[iceman];
				warriors.push_back((Warrior*)new Iceman(this->color));
				Count[iceman]++;
				((Iceman*)warriors.back())->SummonPrint(this->color, this);
				flag = 1;
			}
			break;

		case wolf:
			if (available(pwolf)) {
				totalCount++;
				Health -= Cost[wolf];
				Count[wolf]++;
				warriors.push_back((Warrior*)new Wolf(this->color));
				((Wolf*)warriors.back())->SummonPrint(this->color, this);
				flag = 1;
			}
			break;

		case ninja:
			if (available(pninja)) {
				totalCount++;
				Health -= Cost[ninja];
				Count[ninja]++;
				warriors.push_back((Warrior*)new Ninja(this->color));
				((Ninja*)warriors.back())->SummonPrint(this->color, this);
				flag = 1;
			}
			break;
		case dragon:
			if (available(pdragon)) {
				totalCount++;
				Health -= Cost[dragon];
				Count[dragon]++;
				warriors.push_back((Warrior*)new Dragon(this->color));
				((Dragon*)warriors.back())->SummonPrint(this->color, this);
				flag = 1;
			}
			break;
		case lion:
			if (available(plion)) {
				totalCount++;
				Health -= Cost[lion];
				Count[lion]++;
				warriors.push_back((Warrior*)new Lion(this->color));
				((Lion*)warriors.back())->SummonPrint(this->color, this);
				flag = 1;
			}
			break;
		default:
			break;
		}
		if (flag)
			break;
	}
	if (!flag)
		ableToSummon = false, std::cout << std::setw(3) << std::setfill('0') << Time << " " << (this->color == red ? "red" : "blue") << " headquarter stops making warriors" << std::endl;
	delete pdragon;
	delete plion;
	delete piceman;
	delete pninja;
	delete pwolf;
}

int main()
{
	//	freopen("wow2.in", "r", stdin);
	Red.Order[0] = 2, Red.Order[1] = 3, Red.Order[2] = 4, Red.Order[3] = 1, Red.Order[4] = 0;
	Blue.Order[0] = 3, Blue.Order[1] = 0, Blue.Order[2] = 1, Blue.Order[3] = 2, Blue.Order[4] = 4;
	Red.color = red, Blue.color = blue;
	int testCase;
	std::cin >> testCase;
	int currentTestCase = 0;
	while (currentTestCase < testCase) {
		currentTestCase++;
		Time = 0;
		Red.init(), Blue.init();
		std::cin >> Red.Health;
		Blue.Health = Red.Health;
		std::cin >> Headquarter::Cost[0] >> Headquarter::Cost[1] >> Headquarter::Cost[2] >> Headquarter::Cost[3] >> Headquarter::Cost[4];
		std::cout << "Case:" << currentTestCase << std::endl;
		while (true) {
			Red.Summon();
			Blue.Summon();
			if (!Red.ableToSummon && !Blue.ableToSummon) {
				for (auto i : Red.warriors)
					delete i;
				for (auto i : Blue.warriors)
					delete i;
				break;
			}
			Time++;
		}
	}
}
