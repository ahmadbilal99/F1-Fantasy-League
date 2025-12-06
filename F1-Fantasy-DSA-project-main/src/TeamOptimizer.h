//
// Team Optimizer using Knapsack Dynamic Programming
// Finds the best team within budget constraints
//

#ifndef F1_FANTASY_DSA_TEAMOPTIMIZER_H
#define F1_FANTASY_DSA_TEAMOPTIMIZER_H

#include "Driver.h"
#include "Constructors.h"
#include "Team.h"
#include <vector>
#include <unordered_map>
using namespace std;

struct OptimalTeam {
    vector<Driver*> drivers;
    Constructors* constructor;
    float totalCost;
    int predictedPoints;
    float valueScore;  // Points per million spent
};

class TeamOptimizer {
private:
    vector<Driver*> availableDrivers;
    vector<Constructors*> availableConstructors;
    float budget;

    // DP table for knapsack
    vector<vector<int>> dpTable;

    // Helper functions
    int predictDriverPoints(Driver* driver);
    int predictConstructorPoints(Constructors* constructor);
    float calculateValueScore(float cost, int points);

    // Knapsack DP implementation
    OptimalTeam solveKnapsack(int numDrivers, float driverBudget);

public:
    TeamOptimizer(float budgetLimit);

    // Setup
    void addDriver(Driver* driver);
    void addConstructor(Constructors* constructor);
    void setAvailableDrivers(const vector<Driver*>& drivers);
    void setAvailableConstructors(const vector<Constructors*>& constructors);

    // Optimization
    OptimalTeam findOptimalTeam(int numDrivers = 2);
    vector<OptimalTeam> findTopNTeams(int n, int numDrivers = 2);

    // Display
    void displayOptimalTeam(const OptimalTeam& team);
    void displayTeamComparison(const OptimalTeam& team1, const OptimalTeam& team2);

    // Analysis
    vector<Driver*> getBestValueDrivers(int count = 5);
    vector<Constructors*> getBestValueConstructors(int count = 3);
};

#endif //F1_FANTASY_DSA_TEAMOPTIMIZER_H