//
// Created by hp on 11/30/2025.
//
#include <string>
using namespace std;
#ifndef F1_FANTASY_DSA_CONSTRUCTORS_H
#define F1_FANTASY_DSA_CONSTRUCTORS_H


class Constructors {
private:
    string code;
    string Team_name;
    int points = 0;
    float price;

public:
    Constructors(string Code, string Team,float Price);

    Constructors();

    string getCode();
    string getTeamName();
    int getPoints();
    float getPrice();

    void addpoints(int p);
};


#endif //F1_FANTASY_DSA_CONSTRUCTORS_H