//
// Performance Analyzer Implementation
//

#include "PerformanceAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>

// ============ CONSTRUCTOR ============

PerformanceAnalyzer::PerformanceAnalyzer(int window) : windowSize(window) {}

// ============ CALCULATIONS ============

double PerformanceAnalyzer::calculateMovingAverage(const deque<int>& data) {
    if (data.empty()) return 0.0;

    double sum = 0;
    for (int val : data) {
        sum += val;
    }
    return sum / data.size();
}

double PerformanceAnalyzer::calculateTrend(const deque<int>& data) {
    if (data.size() < 2) return 0.0;

    // Simple linear trend: compare recent half vs older half
    int mid = data.size() / 2;
    double recentAvg = 0, olderAvg = 0;

    for (int i = 0; i < mid; i++) {
        olderAvg += data[i];
    }
    olderAvg /= mid;

    for (int i = mid; i < data.size(); i++) {
        recentAvg += data[i];
    }
    recentAvg /= (data.size() - mid);

    return recentAvg - olderAvg;
}

int PerformanceAnalyzer::predictNextValue(const PerformanceHistory& history) {
    if (history.recentPoints.empty()) return 0;

    // Prediction = moving average + trend adjustment
    double prediction = history.movingAverage + (history.trend * 0.5);

    // Clamp to reasonable range
    if (prediction < 0) prediction = 0;
    if (prediction > 30) prediction = 30;  // Max F1 points per race

    return (int)round(prediction);
}

// ============ ADD RESULTS ============

void PerformanceAnalyzer::addDriverRaceResult(const string& driverCode, int points) {
    PerformanceHistory& history = driverHistory[driverCode];

    // Add new result
    history.recentPoints.push_back(points);

    // Maintain window size
    if (history.recentPoints.size() > windowSize) {
        history.recentPoints.pop_front();
    }

    // Update statistics
    history.movingAverage = calculateMovingAverage(history.recentPoints);
    history.trend = calculateTrend(history.recentPoints);
    history.predictedNextRace = predictNextValue(history);
}

void PerformanceAnalyzer::addConstructorRaceResult(const string& constructorCode, int points) {
    PerformanceHistory& history = constructorHistory[constructorCode];

    history.recentPoints.push_back(points);

    if (history.recentPoints.size() > windowSize) {
        history.recentPoints.pop_front();
    }

    history.movingAverage = calculateMovingAverage(history.recentPoints);
    history.trend = calculateTrend(history.recentPoints);
    history.predictedNextRace = predictNextValue(history);
}

// ============ PREDICTIONS ============

int PerformanceAnalyzer::predictDriverPoints(const string& driverCode) {
    auto it = driverHistory.find(driverCode);
    if (it == driverHistory.end()) {
        return 0;
    }
    return it->second.predictedNextRace;
}

int PerformanceAnalyzer::predictConstructorPoints(const string& constructorCode) {
    auto it = constructorHistory.find(constructorCode);
    if (it == constructorHistory.end()) {
        return 0;
    }
    return it->second.predictedNextRace;
}

// ============ ANALYSIS ============

vector<pair<string, double>> PerformanceAnalyzer::getImprovingDrivers() {
    vector<pair<string, double>> improving;

    for (const auto& pair : driverHistory) {
        if (pair.second.trend > 2.0) {  // Significant improvement
            improving.push_back({pair.first, pair.second.trend});
        }
    }

    // Sort by trend (best improvement first)
    sort(improving.begin(), improving.end(),
         [](const auto& a, const auto& b) {
             return a.second > b.second;
         });

    return improving;
}

vector<pair<string, double>> PerformanceAnalyzer::getDecliningDrivers() {
    vector<pair<string, double>> declining;

    for (const auto& pair : driverHistory) {
        if (pair.second.trend < -2.0) {  // Significant decline
            declining.push_back({pair.first, pair.second.trend});
        }
    }

    // Sort by trend (worst decline first)
    sort(declining.begin(), declining.end(),
         [](const auto& a, const auto& b) {
             return a.second < b.second;
         });

    return declining;
}

string PerformanceAnalyzer::getFormDriver() {
    string bestDriver = "";
    double bestAverage = -1;

    for (const auto& pair : driverHistory) {
        if (pair.second.movingAverage > bestAverage) {
            bestAverage = pair.second.movingAverage;
            bestDriver = pair.first;
        }
    }

    return bestDriver;
}

// ============ DISPLAY ============

void PerformanceAnalyzer::displayDriverTrends() {
    if (driverHistory.empty()) {
        cout << "No driver performance data available!" << endl;
        return;
    }

    cout << "\n=== DRIVER PERFORMANCE TRENDS ===" << endl;
    cout << "Code | Avg Points | Trend | Next Predicted | Status" << endl;
    cout << "--------------------------------------------------------" << endl;

    // Sort by moving average
    vector<pair<string, PerformanceHistory>> sorted;
    for (const auto& pair : driverHistory) {
        sorted.push_back(pair);
    }
    sort(sorted.begin(), sorted.end(),
         [](const auto& a, const auto& b) {
             return a.second.movingAverage > b.second.movingAverage;
         });

    for (const auto& pair : sorted) {
        string status = "→";
        if (pair.second.trend > 2.0) status = "📈";
        else if (pair.second.trend < -2.0) status = "📉";

        cout << setw(4) << pair.first << " | "
             << setw(10) << fixed << setprecision(1) << pair.second.movingAverage << " | "
             << setw(5) << pair.second.trend << " | "
             << setw(14) << pair.second.predictedNextRace << " | "
             << status << endl;
    }
    cout << endl;
}

void PerformanceAnalyzer::displayDriverAnalysis(const string& driverCode) {
    auto it = driverHistory.find(driverCode);
    if (it == driverHistory.end()) {
        cout << "No data for driver: " << driverCode << endl;
        return;
    }

    const PerformanceHistory& history = it->second;

    cout << "\n=== PERFORMANCE ANALYSIS: " << driverCode << " ===" << endl;
    cout << "Races Tracked: " << history.recentPoints.size() << endl;
    cout << "Moving Average: " << fixed << setprecision(2) << history.movingAverage << " points" << endl;
    cout << "Trend: " << history.trend << " points/race ";

    if (history.trend > 2.0) cout << "(📈 Improving)";
    else if (history.trend < -2.0) cout << "(📉 Declining)";
    else cout << "(→ Stable)";
    cout << endl;

    cout << "Next Race Prediction: " << history.predictedNextRace << " points" << endl;

    cout << "\nRecent Race Results: ";
    for (int points : history.recentPoints) {
        cout << points << " ";
    }
    cout << endl;
}

void PerformanceAnalyzer::displayTopFormDrivers(int count) {
    vector<pair<string, double>> formDrivers;

    for (const auto& pair : driverHistory) {
        formDrivers.push_back({pair.first, pair.second.movingAverage});
    }

    sort(formDrivers.begin(), formDrivers.end(),
         [](const auto& a, const auto& b) {
             return a.second > b.second;
         });

    cout << "\n=== TOP " << min(count, (int)formDrivers.size()) << " DRIVERS IN FORM ===" << endl;
    cout << "Rank | Code | Avg Points (Last " << windowSize << " races)" << endl;
    cout << "---------------------------------------------" << endl;

    int limit = min(count, (int)formDrivers.size());
    for (int i = 0; i < limit; i++) {
        cout << setw(4) << (i+1) << " | "
             << setw(4) << formDrivers[i].first << " | "
             << fixed << setprecision(2) << formDrivers[i].second << endl;
    }
    cout << endl;
}

// ============ UTILITY ============

void PerformanceAnalyzer::clear() {
    driverHistory.clear();
    constructorHistory.clear();
}

int PerformanceAnalyzer::getHistorySize(const string& driverCode) {
    auto it = driverHistory.find(driverCode);
    if (it == driverHistory.end()) return 0;
    return it->second.recentPoints.size();
}