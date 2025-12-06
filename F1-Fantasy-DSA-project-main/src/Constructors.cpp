//
// Created by hp on 11/30/2025.
//

#include "Constructors.h"

Constructors::Constructors(string Code, string Team, float Price) {
    code = Code;
    Team_name = Team;
    price = Price;
}

Constructors::Constructors() {
    code = "Default";
    Team_name = "DefaultTeam";
    points = 0;
    price = 0.0f;
}

string Constructors::getCode() {
    return code;
}
int Constructors::getPoints() {
    return points;
}
float Constructors::getPrice() {
    return price;
}
string Constructors::getTeamName() {
    return Team_name;
}

void Constructors::addpoints(int p) {
    points += p;
}




