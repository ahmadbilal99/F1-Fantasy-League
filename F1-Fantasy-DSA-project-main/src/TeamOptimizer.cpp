//
// Team Optimizer Implementation
//

#include "TeamOptimizer.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>

// ============ CONSTRUCTOR ============

TeamOptimizer::TeamOptimizer(float budgetLimit) : budget(budgetLimit) {}

// ============ SETUP ============

void TeamOptimizer::addDriver(Driver* driver) {
    if (driver != nullptr) {
        availableDrivers.push_back(driver);
    }
}

void TeamOptimizer::addConstructor(Constructors* constructor) {
    if (constructor != nullptr) {
        availableConstructors.push_back(constructor);
    }
}

void TeamOptimizer::setAvailableDrivers(const vector<Driver*>& drivers) {
    availableDrivers = drivers;
}

void TeamOptimizer::setAvailableConstructors(const vector<Constructors*>& constructors) {
    availableConstructors = constructors;
}

// ============ PREDICTION FUNCTIONS ============

int TeamOptimizer::predictDriverPoints(Driver* driver) {
    // Simple prediction: current points + 10% growth
    // In real implementation, could use moving average or regression
    return driver->getPoints() + (driver->getPoints() / 10);
}

int TeamOptimizer::predictConstructorPoints(Constructors* constructor) {
    // Predict based on current performance
    return constructor->getPoints() + (constructor->getPoints() / 10);
}

float TeamOptimizer::calculateValueScore(float cost, int points) {
    if (cost <= 0) return 0;
    return points / cost;
}

// ============ KNAPSACK OPTIMIZATION ============

OptimalTeam TeamOptimizer::findOptimalTeam(int numDrivers) {
    OptimalTeam bestTeam;
    bestTeam.totalCost = 0;
    bestTeam.predictedPoints = 0;
    bestTeam.constructor = nullptr;

    if (availableDrivers.empty() || availableConstructors.empty()) {
        cout << "Not enough drivers or constructors available!" << endl;
        return bestTeam;
    }

    // Try each constructor
    for (Constructors* constructor : availableConstructors) {
        float constructorCost = constructor->getPrice();

        // Check if constructor fits budget
        if (constructorCost >= budget) continue;

        float driverBudget = budget - constructorCost;

        // Find best drivers with remaining budget using knapsack
        OptimalTeam teamWithConstructor = solveKnapsack(numDrivers, driverBudget);

        // Add constructor to team
        teamWithConstructor.constructor = constructor;
        teamWithConstructor.totalCost += constructorCost;
        teamWithConstructor.predictedPoints += predictConstructorPoints(constructor);
        teamWithConstructor.valueScore = calculateValueScore(
            teamWithConstructor.totalCost,
            teamWithConstructor.predictedPoints
        );

        // Update best team if this is better
        if (teamWithConstructor.predictedPoints > bestTeam.predictedPoints) {
            bestTeam = teamWithConstructor;
        }
    }

    return bestTeam;
}

OptimalTeam TeamOptimizer::solveKnapsack(int numDrivers, float driverBudget) {
    OptimalTeam team;
    team.totalCost = 0;
    team.predictedPoints = 0;
    team.constructor = nullptr;

    int n = availableDrivers.size();
    int W = (int)(driverBudget * 10);  // Convert to integer (multiply by 10 for precision)

    // Create DP table: dp[i][w] = max points with first i drivers and budget w
    dpTable.assign(n + 1, vector<int>(W + 1, 0));

    // Fill DP table
    for (int i = 1; i <= n; i++) {
        Driver* driver = availableDrivers[i - 1];
        int cost = (int)(driver->getPrice() * 10);
        int points = predictDriverPoints(driver);

        for (int w = 0; w <= W; w++) {
            // Don't take this driver
            dpTable[i][w] = dpTable[i - 1][w];

            // Take this driver if affordable
            if (cost <= w) {
                dpTable[i][w] = max(dpTable[i][w], dpTable[i - 1][w - cost] + points);
            }
        }
    }

    // Backtrack to find which drivers were selected
    int w = W;
    vector<bool> selected(n, false);

    for (int i = n; i > 0 && w > 0; i--) {
        if (dpTable[i][w] != dpTable[i - 1][w]) {
            Driver* driver = availableDrivers[i - 1];
            selected[i - 1] = true;
            w -= (int)(driver->getPrice() * 10);
        }
    }

    // Build team from selected drivers (limited to numDrivers)
    int count = 0;
    for (int i = 0; i < n && count < numDrivers; i++) {
        if (selected[i]) {
            team.drivers.push_back(availableDrivers[i]);
            team.totalCost += availableDrivers[i]->getPrice();
            team.predictedPoints += predictDriverPoints(availableDrivers[i]);
            count++;
        }
    }

    return team;
}

vector<OptimalTeam> TeamOptimizer::findTopNTeams(int n, int numDrivers) {
    vector<OptimalTeam> teams;

    // Generate teams with different constructors
    for (Constructors* constructor : availableConstructors) {
        float constructorCost = constructor->getPrice();
        if (constructorCost >= budget) continue;

        float driverBudget = budget - constructorCost;
        OptimalTeam team = solveKnapsack(numDrivers, driverBudget);

        team.constructor = constructor;
        team.totalCost += constructorCost;
        team.predictedPoints += predictConstructorPoints(constructor);
        team.valueScore = calculateValueScore(team.totalCost, team.predictedPoints);

        teams.push_back(team);
    }

    // Sort by predicted points
    sort(teams.begin(), teams.end(),
         [](const OptimalTeam& a, const OptimalTeam& b) {
             return a.predictedPoints > b.predictedPoints;
         });

    // Return top N
    if (teams.size() > n) {
        teams.resize(n);
    }

    return teams;
}

// ============ ANALYSIS ============

vector<Driver*> TeamOptimizer::getBestValueDrivers(int count) {
    vector<pair<float, Driver*>> valueDrivers;

    for (Driver* driver : availableDrivers) {
        float valueScore = calculateValueScore(driver->getPrice(), predictDriverPoints(driver));
        valueDrivers.push_back({valueScore, driver});
    }

    sort(valueDrivers.begin(), valueDrivers.end(),
         [](const pair<float, Driver*>& a, const pair<float, Driver*>& b) {
             return a.first > b.first;
         });

    vector<Driver*> result;
    int limit = min(count, (int)valueDrivers.size());
    for (int i = 0; i < limit; i++) {
        result.push_back(valueDrivers[i].second);
    }

    return result;
}

vector<Constructors*> TeamOptimizer::getBestValueConstructors(int count) {
    vector<pair<float, Constructors*>> valueConstructors;

    for (Constructors* constructor : availableConstructors) {
        float valueScore = calculateValueScore(
            constructor->getPrice(),
            predictConstructorPoints(constructor)
        );
        valueConstructors.push_back({valueScore, constructor});
    }

    sort(valueConstructors.begin(), valueConstructors.end(),
         [](const pair<float, Constructors*>& a, const pair<float, Constructors*>& b) {
             return a.first > b.first;
         });

    vector<Constructors*> result;
    int limit = min(count, (int)valueConstructors.size());
    for (int i = 0; i < limit; i++) {
        result.push_back(valueConstructors[i].second);
    }

    return result;
}

// ============ DISPLAY ============

void TeamOptimizer::displayOptimalTeam(const OptimalTeam& team) {
    if (team.drivers.empty()) {
        cout << "No valid team found within budget!" << endl;
        return;
    }

    cout << "\n=== OPTIMAL TEAM SUGGESTION ===" << endl;
    cout << "Budget: " << budget << "M" << endl;
    cout << "Total Cost: " << team.totalCost << "M" << endl;
    cout << "Remaining: " << (budget - team.totalCost) << "M" << endl;
    cout << "Predicted Points: " << team.predictedPoints << endl;
    cout << "Value Score: " << fixed << setprecision(2) << team.valueScore << " pts/M" << endl;

    cout << "\nConstructor:" << endl;
    if (team.constructor != nullptr) {
        cout << "  • " << team.constructor->getTeamName()
             << " - " << team.constructor->getPrice() << "M"
             << " (Predicted: " << predictConstructorPoints(team.constructor) << " pts)" << endl;
    }

    cout << "\nDrivers:" << endl;
    for (Driver* driver : team.drivers) {
        cout << "  • " << driver->getName()
             << " (" << driver->getTeam() << ") - " << driver->getPrice() << "M"
             << " (Predicted: " << predictDriverPoints(driver) << " pts)" << endl;
    }
    cout << endl;
}

void TeamOptimizer::displayTeamComparison(const OptimalTeam& team1, const OptimalTeam& team2) {
    cout << "\n=== TEAM COMPARISON ===" << endl;
    cout << left << setw(25) << "Metric" << setw(20) << "Team 1" << "Team 2" << endl;
    cout << "-----------------------------------------------------------" << endl;

    cout << setw(25) << "Total Cost:"
         << setw(20) << (to_string((int)team1.totalCost) + "M")
         << (to_string((int)team2.totalCost) + "M") << endl;

    cout << setw(25) << "Predicted Points:"
         << setw(20) << team1.predictedPoints
         << team2.predictedPoints << endl;

    cout << setw(25) << "Value Score:"
         << setw(20) << fixed << setprecision(2) << team1.valueScore
         << team2.valueScore << endl;

    if (team1.constructor && team2.constructor) {
        cout << setw(25) << "Constructor:"
             << setw(20) << team1.constructor->getTeamName()
             << team2.constructor->getTeamName() << endl;
    }

    cout << endl;
}