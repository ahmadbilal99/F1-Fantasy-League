//
// Created by hp on 11/30/2025.
//

#include "User.h"

User::User(string username, float Budget, Team team) {
    this->username = username;
    this->budget = Budget;
    this->team = team;
}

void User::addPoints(int p) {
    points += p;
}

string User::getUsername() {
    return username;
}
float User::getBudget() {
    return budget;
}
Team User::getTeam() {
    return team;
}
int User::getPoints() {
    return points;
}





