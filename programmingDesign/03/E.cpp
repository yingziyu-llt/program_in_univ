/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
using namespace std;

class CWarrior {
public:
	string name;
	int cost;
	int RedCount;		  // this kind of warrior
	int BlueCount;		  // this kind of warrior
	static int RedTotal;  // all kind of warrior
	static int BlueTotal; // all kind of warrior
	static int RedMoney;
	static int Time;
	static int BlueMoney;
	CWarrior(string, int);
	CWarrior();
	void addANewWarrior(int); // 0 represents red, 1 represents blue
	bool avilable(int);
};
int CWarrior::BlueMoney = 0;
int CWarrior::RedMoney = 0;
int CWarrior::Time = 0;
int CWarrior::RedTotal = 0;
int CWarrior::BlueTotal = 0;
CWarrior::CWarrior(string uName, int uCost)
{
	name = uName;
	cost = uCost;
	RedCount = 0;
	BlueCount = 0;
}

CWarrior::CWarrior() { }

void CWarrior::addANewWarrior(int color)
{
	if (color == 0) {
		RedCount++, RedMoney -= cost;
		RedTotal++;
		cout.width(3), cout.fill('0');
		cout << Time;
		cout << " red " << name << " " << RedTotal << " born with strength " << cost << "," << RedCount << " " << name << " in red headquarter";
	} else {
		BlueCount++, BlueMoney -= cost;
		BlueTotal++;
		cout.width(3), cout.fill('0');
		cout << Time;
		cout << " blue " << name << " " << BlueTotal << " born with strength " << cost << "," << BlueCount << " " << name << " in blue headquarter";
	}
	cout << endl;
}

bool CWarrior::avilable(int color)
{
	if (color == 0) {
		if (RedMoney >= cost)
			return true;
		else
			return false;
	} else {
		if (BlueMoney >= cost)
			return true;
		else
			return false;
	}
}

const int RedOrder[] = { 2, 3, 4, 1, 0 };
const int BlueOrder[] = { 3, 0, 1, 2, 4 };

int main()
{
	int T;
	scanf("%d", &T);
	int testCount = 1;
	for (; testCount <= T; testCount++) {
		CWarrior::Time = 0;
		CWarrior::RedTotal = 0;
		CWarrior::BlueTotal = 0;
		cout << "Case:" << testCount << endl;
		int M = 0;
		cin >> M;
		CWarrior::RedMoney = M, CWarrior::BlueMoney = M;
		int a, b, c, d, e;
		cin >> a >> b >> c >> d >> e;
		CWarrior Warriors[5] = { CWarrior("dragon", a), CWarrior("ninja", b), CWarrior("iceman", c), CWarrior("lion", d), CWarrior("wolf", e) };
		bool BlueOK = false;
		bool RedOK = false;
		int currentRedOrder = 0, currentBlueOrder = 0;
		while (1) {
			bool BlueFinished = false;
			bool RedFinished = false;
			int tryCount = 0;
			while (1 && !(RedOK)) {
				if (tryCount >= 5)
					break;
				if (Warriors[RedOrder[currentRedOrder]].avilable(0)) {
					Warriors[RedOrder[currentRedOrder]].addANewWarrior(0);
					RedFinished = 1;
					tryCount++, currentRedOrder = (currentRedOrder + 1) % 5;
					break;
				}
				tryCount++, currentRedOrder = (currentRedOrder + 1) % 5;
			}
			if (!RedFinished && !RedOK) {
				RedOK = 1;
				cout.width(3), cout.fill('0');
				cout << CWarrior::Time << " red headquarter stops making warriors" << endl;
			}
			tryCount = 0;

			while (1 && !(BlueOK)) {
				if (tryCount >= 5)
					break;
				if (Warriors[BlueOrder[currentBlueOrder]].avilable(1)) {
					Warriors[BlueOrder[currentBlueOrder]].addANewWarrior(1);
					BlueFinished = 1;
					tryCount++, currentBlueOrder = (currentBlueOrder + 1) % 5;
					break;
				}
				tryCount++, currentBlueOrder = (currentBlueOrder + 1) % 5;
			}
			if (!BlueFinished && !BlueOK) {
				BlueOK = 1;
				cout.width(3), cout.fill('0');
				cout << CWarrior::Time << " blue headquarter stops making warriors" << endl;
			}
			if (BlueOK && RedOK)
				break;
			CWarrior::Time++;
		}
	}
}
