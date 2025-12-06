//
// Created by hp on 11/30/2025.
//
#include <string>

#include "Team.h"
using namespace std;
#ifndef F1_FANTASY_DSA_USER_H
#define F1_FANTASY_DSA_USER_H


class User {
private:
    string username;
    int points = 0;
    float budget;
    Team team = Team();

    public:
    User();
    User(string username, float Budget, Team team);

    string getUsername();
    int getPoints();
    float getBudget();
    Team getTeam();

    void addPoints(int p);
};


#endif //F1_FANTASY_DSA_USER_H