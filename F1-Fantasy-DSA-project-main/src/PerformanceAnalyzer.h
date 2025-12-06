//
// Performance Analyzer - Moving Average for Predictions
// Tracks driver performance trends over time
//

#ifndef F1_FANTASY_DSA_PERFORMANCEANALYZER_H
#define F1_FANTASY_DSA_PERFORMANCEANALYZER_H

#include "Driver.h"
#include "Constructors.h"
#include <vector>
#include <deque>
#include <unordered_map>
#include <string>
using namespace std;

struct PerformanceHistory {
    deque<int> recentPoints;  // Last N races
    double movingAverage;
    double trend;  // Positive = improving, negative = declining
    int predictedNextRace;
};

class PerformanceAnalyzer {
private:
    unordered_map<string, PerformanceHistory> driverHistory;
    unordered_map<string, PerformanceHistory> constructorHistory;
    int windowSize;  // How many races to consider for moving average

    // Helper functions
    double calculateMovingAverage(const deque<int>& data);
    double calculateTrend(const deque<int>& data);
    int predictNextValue(const PerformanceHistory& history);

public:
    PerformanceAnalyzer(int window = 5);

    // Add race results
    void addDriverRaceResult(const string& driverCode, int points);
    void addConstructorRaceResult(const string& constructorCode, int points);

    // Predictions
    int predictDriverPoints(const string& driverCode);
    int predictConstructorPoints(const string& constructorCode);

    // Analysis
    vector<pair<string, double>> getImprovingDrivers();
    vector<pair<string, double>> getDecliningDrivers();
    string getFormDriver();  // Driver in best recent form

    // Display
    void displayDriverTrends();
    void displayDriverAnalysis(const string& driverCode);
    void displayTopFormDrivers(int count = 5);

    // Utility
    void clear();
    int getHistorySize(const string& driverCode);
};

#endif //F1_FANTASY_DSA_PERFORMANCEANALYZER_H