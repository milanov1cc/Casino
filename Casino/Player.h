#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <string>

using namespace std;

class Player
{
public:
	string name;
	double balance;

	Player();
	Player(string, double);
	Player(Player&, int);
	int getPlayerID();
	string getName();
	double getBalnce();
	void setBalance(int x);

	friend istream& operator >> (istream&, Player&);
	friend ostream& operator << (ostream&, Player);
};

const int playerID = 1;

#endif