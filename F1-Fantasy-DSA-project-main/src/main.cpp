#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>


#include "Constructors.h"
#include "Driver.h"
#include "Team.h"
#include "User.h"
#include "Sorting.h"
#include "AVLTree.h"  // NEW: Include AVL Tree
#include "Trie.h"
#include "RaceSimulator.h"
#include "TeamOptimizer.h"
#include "Leaderboard.h"
#include "PerformanceAnalyzer.h"
#include "AlertSystem.h"

using namespace std;

vector<Driver> LoadDrivers(string filename);
vector<Constructors> LoadConstructors(string filename);
Driver SelectDriver(const vector<Driver> &drivers);
Constructors SelectConstructor(const vector<Constructors> &constructors);
bool StoreUserTeam(string filename, User user);
void displaySortingMenu(vector<Driver>& drivers, vector<Constructors>& constructors);
void displayAVLMenu(AVLTree& avlTree);  // NEW
void displayMainMenu();

int main() {
    srand(time(0));

    // Load data
    vector<Driver> drivers = LoadDrivers("../data/drivers.csv");
    vector<Constructors> constructors = LoadConstructors("../data/constructors.csv");

    // Add some initial points for testing
    if (!drivers.empty()) {
        for (int i = 0; i < drivers.size(); i++) {
            int points = rand() % 200 + 50;
            drivers[i].addpoints(points);
        }
    }

    if (!constructors.empty()) {
        for (int i = 0; i < constructors.size(); i++) {
            int points = rand() % 300 + 100;
            constructors[i].addpoints(points);
        }
    }

    // NEW: Create and populate AVL Tree
    AVLTree avlTree;
    cout << "Building AVL Tree for driver rankings..." << endl;
    for (Driver& d : drivers) {
        avlTree.insert(d);
    }
    cout << "AVL Tree built with " << avlTree.size() << " drivers!" << endl;

    // After building AVL tree, add:
    Trie driverTrie;
    cout << "Building Trie for driver search..." << endl;
    for (Driver& d : drivers) {
        driverTrie.insert(&d);
    }
    cout << "Trie built with " << drivers.size() << " drivers!" << endl;

    // After building Trie, add:
    Leaderboard leaderboard;
    cout << "Leaderboard system initialized!" << endl;

    // After leaderboard initialization:
PerformanceAnalyzer analyzer(5);  // 5-race moving average
AlertSystem alertSystem(100);     // Keep last 100 alerts
cout << "Performance analyzer initialized!" << endl;
cout << "Alert system initialized!" << endl;

// Add welcome alert
alertSystem.addAlert(MEDIUM, SYSTEM_MESSAGE, "Welcome to F1 Fantasy League!", "System");


    // Main program loop
    while (true) {
        displayMainMenu();

        int choice;
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(choice) {
            case 1: {
                // Create New Team
                string username;
                float budget;

                cout << "\n=== CREATE NEW TEAM ===" << endl;
                cout << "Enter username: ";
                getline(cin, username);

                cout << "Enter your budget (in millions): ";
                cin >> budget;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                Constructors chosenConstructor = SelectConstructor(constructors);
                Team userTeam(budget, chosenConstructor);

                int maxDrivers = 2;

                for (int i = 0; i < maxDrivers; ++i) {
                    Driver d = SelectDriver(drivers);
                    userTeam.addDriver(d);
                    cout << d.getName() << " added to your team!\n";
                }

                User user(username, budget, userTeam);
                vector<Driver> teamDrivers = userTeam.getDrivers();

                cout << "\n=== YOUR TEAM ===" << endl;
                cout << "Username: " << username << endl;
                cout << "Budget: " << budget << "M" << endl;
                cout << "Constructor: " << chosenConstructor.getTeamName()
                     << " (" << chosenConstructor.getPrice() << "M)" << endl;
                cout << "Drivers: ";
                for (Driver& d : teamDrivers) {
                    cout << d.getName() << " (" << d.getPrice() << "M) ";
                }
                cout << endl;

                float teamCost = chosenConstructor.getPrice();
                for (Driver& d : teamDrivers) {
                    teamCost += d.getPrice();
                }
                cout << "Total Team Cost: " << teamCost << "M / " << budget << "M" << endl;

                StoreUserTeam("../data/users.csv", user);
                break;

                    leaderboard.addUser(&user);
                    cout << "✅ Team added to leaderboard!" << endl;
                    break;
            }
            case 2:
                // QuickSort Rankings
                displaySortingMenu(drivers, constructors);
                break;
            case 3: {
                // QuickSort Performance Test
                cout << "\n=== QUICKSORT PERFORMANCE TEST ===" << endl;
                vector<Driver> testDrivers = drivers;

                cout << "Testing QuickSort on " << testDrivers.size() << " drivers..." << endl;

                clock_t start = clock();
                SortingUtils::quickSortDriversByPoints(testDrivers);
                clock_t end = clock();
                double sortTime = double(end - start) / CLOCKS_PER_SEC;

                cout << "Sorted in " << sortTime << " seconds" << endl;

                bool sorted = true;
                for (size_t i = 0; i < testDrivers.size() - 1; i++) {
                    if (testDrivers[i].getPoints() < testDrivers[i + 1].getPoints()) {
                        sorted = false;
                        break;
                    }
                }
                cout << "Correctly sorted (descending by points): "
                     << (sorted ? "YES" : "NO") << endl;
                break;
            }
            case 4:
                // NEW: AVL Tree Rankings
                displayAVLMenu(avlTree);
                break;
            case 5: {
                // NEW: Update driver points and rebuild AVL tree
                cout << "\n=== UPDATE DRIVER POINTS ===" << endl;

                string code;
                cout << "Enter driver code: ";
                getline(cin, code);

                // Find driver in vector
                bool found = false;
                for (Driver& d : drivers) {
                    if (d.getCode() == code) {
                        int points;
                        cout << "Current points: " << d.getPoints() << endl;
                        cout << "Enter points to add: ";
                        cin >> points;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        d.addpoints(points);
                        cout << "New points: " << d.getPoints() << endl;

                        // Rebuild AVL tree
                        cout << "Rebuilding AVL tree..." << endl;
                        avlTree.clear();
                        for (Driver& dr : drivers) {
                            avlTree.insert(dr);
                        }
                        cout << "AVL tree updated!" << endl;

                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cout << "Driver not found!" << endl;
                }
                break;
            }
            case 6: {
    // Trie Search Menu
    int searchChoice;
    do {
        cout << "\n=== DRIVER SEARCH (TRIE) ===" << endl;
        cout << "1. Search by Name Prefix" << endl;
        cout << "2. Search by Driver Code" << endl;
        cout << "3. Search Exact Name" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Choose option: ";

        // FIX: Read the choice properly
        if (!(cin >> searchChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(searchChoice) {
            case 1: {
                string prefix;
                cout << "Enter name prefix (e.g., 'Max', 'Lew'): ";
                getline(cin, prefix);

                vector<string> suggestions = driverTrie.searchPrefix(prefix);

                if (suggestions.empty()) {
                    cout << "No drivers found with prefix '" << prefix << "'" << endl;
                } else {
                    cout << "\n=== SEARCH RESULTS ===" << endl;
                    cout << "Found " << suggestions.size() << " driver(s):" << endl;
                    for (const string& suggestion : suggestions) {
                        cout << "  • " << suggestion << endl;
                    }
                }
                break;
            }

            case 2: {
                string code;
                cout << "Enter driver code (e.g., 'VER', 'HAM'): ";
                getline(cin, code);

                vector<Driver*> foundDrivers = driverTrie.searchDriversByPrefix(code);

                if (foundDrivers.empty()) {
                    cout << "No drivers found with code '" << code << "'" << endl;
                } else {
                    cout << "\n=== DRIVER DETAILS ===" << endl;
                    for (Driver* d : foundDrivers) {
                        cout << "Code: " << d->getCode() << endl;
                        cout << "Name: " << d->getName() << endl;
                        cout << "Team: " << d->getTeam() << endl;
                        cout << "Points: " << d->getPoints() << endl;
                        cout << "Price: " << d->getPrice() << "M" << endl;
                        cout << "-------------------" << endl;
                    }
                }
                break;
            }

            case 3: {
                string name;
                cout << "Enter exact driver name: ";
                getline(cin, name);

                Driver* found = driverTrie.searchExact(name);

                if (found == nullptr) {
                    cout << "Driver not found!" << endl;
                } else {
                    cout << "\n=== DRIVER FOUND ===" << endl;
                    cout << "Code: " << found->getCode() << endl;
                    cout << "Name: " << found->getName() << endl;
                    cout << "Team: " << found->getTeam() << endl;
                    cout << "Points: " << found->getPoints() << endl;
                    cout << "Price: " << found->getPrice() << "M" << endl;
                }
                break;
            }

            case 4:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice! Please enter 1-4." << endl;
        }

        if (searchChoice != 4) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (searchChoice != 4);
    break;
}
            case 7: {
    // Race Simulation
    cout << "\n=== RACE SIMULATION ===" << endl;

    string raceName;
    cout << "Enter race name (e.g., 'Monaco GP', 'Silverstone'): ";
    getline(cin, raceName);

    RaceSimulator race(raceName);

    // Add all drivers to race
    for (Driver& d : drivers) {
        race.addDriver(&d);
    }

    cout << "\nStarting race with " << drivers.size() << " drivers..." << endl;
    cout << "Press Enter to start the race...";
    cin.get();

    // Run the race
    race.runRace();

    // Get results
    vector<RaceResult> results = race.getResults();

    // Display results
    race.displayResults();
    race.displayDetailedResults();

    // Track performance for analyzer
    cout << "\n📊 Updating performance statistics..." << endl;
    for (const RaceResult& result : results) {
        analyzer.addDriverRaceResult(
            result.driver->getCode(),
            result.pointsEarned
        );
    }

    // Generate alerts for notable performances
    RaceResult winner = race.getWinner();
    if (winner.driver != nullptr) {
        alertSystem.addRaceResult(raceName, winner.driver->getName());
    }

    Driver* fastestLap = race.getFastestLapDriver();
    if (fastestLap != nullptr) {
        alertSystem.addAlert(
            MEDIUM,
            RACE_RESULT,
            fastestLap->getName() + " scored fastest lap in " + raceName,
            fastestLap->getCode()
        );
    }

    // Check for drivers with exceptional results
    for (const RaceResult& result : results) {
        if (result.position == 1 && result.pointsEarned >= 25) {
            alertSystem.addAlert(
                HIGH,
                RACE_RESULT,
                result.driver->getName() + " WON " + raceName + "!",
                result.driver->getCode()
            );
        }
    }

    // Update AVL tree with new points
    cout << "Updating driver rankings..." << endl;
    avlTree.clear();
    for (Driver& d : drivers) {
        avlTree.insert(d);
    }

    cout << "\n✅ Race complete! Points updated!" << endl;
    cout << "📊 Performance data recorded for analysis" << endl;
    cout << "📬 New alerts generated" << endl;
    break;
}
            case 8: {
    // Team Builder with Knapsack
    int optimizerChoice;
    do {
        cout << "\n=== TEAM BUILDER (KNAPSACK DP) ===" << endl;
        cout << "1. Find Optimal Team" << endl;
        cout << "2. Compare Top 3 Teams" << endl;
        cout << "3. Best Value Drivers" << endl;
        cout << "4. Best Value Constructors" << endl;
        cout << "5. Custom Budget Analysis" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Choose option: ";

        if (!(cin >> optimizerChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(optimizerChoice) {
            case 1: {
                float budget;
                cout << "Enter your budget (in millions): ";
                cin >> budget;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                TeamOptimizer optimizer(budget);

                // Add drivers and constructors
                for (Driver& d : drivers) {
                    optimizer.addDriver(&d);
                }
                for (Constructors& c : constructors) {
                    optimizer.addConstructor(&c);
                }

                cout << "\n🔄 Running Knapsack algorithm..." << endl;
                cout << "Analyzing " << drivers.size() << " drivers and "
                     << constructors.size() << " constructors..." << endl;

                OptimalTeam optimalTeam = optimizer.findOptimalTeam(2);
                optimizer.displayOptimalTeam(optimalTeam);

                // Ask if user wants to create this team
                cout << "Create this team? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (choice == 'y' || choice == 'Y') {
                    string username;
                    cout << "Enter username: ";
                    getline(cin, username);

                    Team newTeam(budget, *optimalTeam.constructor);
                    for (Driver* d : optimalTeam.drivers) {
                        newTeam.addDriver(*d);
                    }

                    User newUser(username, budget, newTeam);
                    leaderboard.addUser(&newUser);
                    StoreUserTeam("../data/users.csv", newUser);

                    cout << "✅ Optimal team created and added to leaderboard!" << endl;
                }
                break;
            }

            case 2: {
                float budget;
                cout << "Enter your budget (in millions): ";
                cin >> budget;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                TeamOptimizer optimizer(budget);
                for (Driver& d : drivers) {
                    optimizer.addDriver(&d);
                }
                for (Constructors& c : constructors) {
                    optimizer.addConstructor(&c);
                }

                cout << "\n🔄 Finding top 3 team configurations..." << endl;
                vector<OptimalTeam> topTeams = optimizer.findTopNTeams(3, 2);

                cout << "\n=== TOP 3 TEAM CONFIGURATIONS ===" << endl;
                for (int i = 0; i < topTeams.size(); i++) {
                    cout << "\n--- OPTION " << (i+1) << " ---" << endl;
                    optimizer.displayOptimalTeam(topTeams[i]);
                }

                if (topTeams.size() >= 2) {
                    cout << "\n=== COMPARISON: Option 1 vs Option 2 ===" << endl;
                    optimizer.displayTeamComparison(topTeams[0], topTeams[1]);
                }
                break;
            }

            case 3: {
                float budget;
                cout << "Enter budget for analysis: ";
                cin >> budget;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                TeamOptimizer optimizer(budget);
                for (Driver& d : drivers) {
                    optimizer.addDriver(&d);
                }

                vector<Driver*> bestValue = optimizer.getBestValueDrivers(5);

                cout << "\n=== BEST VALUE DRIVERS ===" << endl;
                cout << "Rank | Driver                | Price | Predicted Pts | Value Score" << endl;
                cout << "---------------------------------------------------------------------" << endl;

                for (int i = 0; i < bestValue.size(); i++) {
                    Driver* d = bestValue[i];
                    int predicted = d->getPoints() + (d->getPoints() / 10);
                    float valueScore = predicted / d->getPrice();

                    cout << setw(4) << (i+1) << " | "
                         << setw(21) << left << d->getName() << " | "
                         << setw(5) << right << d->getPrice() << "M | "
                         << setw(13) << predicted << " | "
                         << fixed << setprecision(2) << valueScore << endl;
                }
                break;
            }

            case 4: {
                float budget;
                cout << "Enter budget for analysis: ";
                cin >> budget;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                TeamOptimizer optimizer(budget);
                for (Constructors& c : constructors) {
                    optimizer.addConstructor(&c);
                }

                vector<Constructors*> bestValue = optimizer.getBestValueConstructors(3);

                cout << "\n=== BEST VALUE CONSTRUCTORS ===" << endl;
                cout << "Rank | Constructor       | Price | Predicted Pts | Value Score" << endl;
                cout << "---------------------------------------------------------------------" << endl;

                for (int i = 0; i < bestValue.size(); i++) {
                    Constructors* c = bestValue[i];
                    int predicted = c->getPoints() + (c->getPoints() / 10);
                    float valueScore = predicted / c->getPrice();

                    cout << setw(4) << (i+1) << " | "
                         << setw(17) << left << c->getTeamName() << " | "
                         << setw(5) << right << c->getPrice() << "M | "
                         << setw(13) << predicted << " | "
                         << fixed << setprecision(2) << valueScore << endl;
                }
                break;
            }

            case 5: {
                cout << "\n=== CUSTOM BUDGET ANALYSIS ===" << endl;
                float minBudget, maxBudget, step;

                cout << "Enter minimum budget: ";
                cin >> minBudget;
                cout << "Enter maximum budget: ";
                cin >> maxBudget;
                cout << "Enter step size: ";
                cin >> step;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "\nBudget | Predicted Points | Constructor" << endl;
                cout << "---------------------------------------------" << endl;

                for (float b = minBudget; b <= maxBudget; b += step) {
                    TeamOptimizer optimizer(b);
                    for (Driver& d : drivers) {
                        optimizer.addDriver(&d);
                    }
                    for (Constructors& c : constructors) {
                        optimizer.addConstructor(&c);
                    }

                    OptimalTeam team = optimizer.findOptimalTeam(2);

                    cout << setw(6) << b << "M | "
                         << setw(16) << team.predictedPoints << " | ";
                    if (team.constructor) {
                        cout << team.constructor->getTeamName();
                    }
                    cout << endl;
                }
                break;
            }

            case 6:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice!" << endl;
        }

        if (optimizerChoice != 6) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (optimizerChoice != 6);
    break;
}
            case 10: {
    // Performance Analyzer
    int analyzerChoice;
    do {
        cout << "\n=== PERFORMANCE ANALYZER ===" << endl;
        cout << "1. View All Driver Trends" << endl;
        cout << "2. Analyze Specific Driver" << endl;
        cout << "3. Top Drivers in Form" << endl;
        cout << "4. Improving Drivers" << endl;
        cout << "5. Declining Drivers" << endl;
        cout << "6. Next Race Predictions" << endl;
        cout << "7. Back to Main Menu" << endl;
        cout << "Choose option: ";

        if (!(cin >> analyzerChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(analyzerChoice) {
            case 1:
                analyzer.displayDriverTrends();
                break;

            case 2: {
                string code;
                cout << "Enter driver code: ";
                getline(cin, code);
                analyzer.displayDriverAnalysis(code);
                break;
            }

            case 3:
                analyzer.displayTopFormDrivers(5);
                break;

            case 4: {
                vector<pair<string, double>> improving = analyzer.getImprovingDrivers();

                if (improving.empty()) {
                    cout << "No significantly improving drivers found." << endl;
                } else {
                    cout << "\n=== 📈 IMPROVING DRIVERS ===" << endl;
                    cout << "Code | Trend (pts/race)" << endl;
                    cout << "-------------------------" << endl;

                    for (const auto& pair : improving) {
                        cout << setw(4) << pair.first << " | +"
                             << fixed << setprecision(2) << pair.second << endl;

                        // Generate alert for significantly improving drivers
                        if (pair.second > 5.0) {
                            alertSystem.addFormAlert(pair.first, "Significantly Improving!");
                        }
                    }
                }
                break;
            }

            case 5: {
                vector<pair<string, double>> declining = analyzer.getDecliningDrivers();

                if (declining.empty()) {
                    cout << "No significantly declining drivers found." << endl;
                } else {
                    cout << "\n=== 📉 DECLINING DRIVERS ===" << endl;
                    cout << "Code | Trend (pts/race)" << endl;
                    cout << "-------------------------" << endl;

                    for (const auto& pair : declining) {
                        cout << setw(4) << pair.first << " | "
                             << fixed << setprecision(2) << pair.second << endl;

                        // Generate alert for significantly declining drivers
                        if (pair.second < -5.0) {
                            alertSystem.addFormAlert(pair.first, "Form Declining");
                        }
                    }
                }
                break;
            }

            case 6: {
                cout << "\n=== NEXT RACE PREDICTIONS ===" << endl;
                cout << "Code | Current Avg | Prediction" << endl;
                cout << "------------------------------------" << endl;

                for (Driver& d : drivers) {
                    int prediction = analyzer.predictDriverPoints(d.getCode());
                    if (analyzer.getHistorySize(d.getCode()) >= 3) {
                        cout << setw(4) << d.getCode() << " | "
                             << setw(11) << (d.getPoints() / max(1, analyzer.getHistorySize(d.getCode())))
                             << " | " << prediction << " pts" << endl;
                    }
                }
                break;
            }

            case 7:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice!" << endl;
        }

        if (analyzerChoice != 7) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (analyzerChoice != 7);
    break;
}
                case 11: {
    // Alert Center
    int alertChoice;
    do {
        cout << "\n=== ALERT CENTER ===" << endl;
        cout << "Unread: " << alertSystem.getUnreadCount()
             << " | Critical: " << alertSystem.getCriticalCount() << endl;
        cout << "\n1. View All Alerts" << endl;
        cout << "2. View Unread Alerts" << endl;
        cout << "3. View Critical Alerts" << endl;
        cout << "4. View Alert History" << endl;
        cout << "5. Create Test Alerts" << endl;
        cout << "6. Clear All Alerts" << endl;
        cout << "7. Back to Main Menu" << endl;
        cout << "Choose option: ";

        if (!(cin >> alertChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(alertChoice) {
            case 1:
                alertSystem.displayAlerts();
                break;

            case 2:
                alertSystem.displayUnreadAlerts();
                break;

            case 3:
                alertSystem.displayAlertsByPriority(CRITICAL);
                break;

            case 4:
                alertSystem.displayAlertHistory();
                break;

            case 5: {
                // Create demo alerts
                cout << "\nGenerating test alerts..." << endl;

                alertSystem.addAlert(
                    CRITICAL,
                    DRIVER_INJURY,
                    "Max Verstappen ruled out due to injury",
                    "VER"
                );

                alertSystem.addPriceChange("Lewis Hamilton", 25.5, 27.0);
                alertSystem.addPriceChange("Lando Norris", 20.0, 19.5);

                alertSystem.addFormAlert("Charles Leclerc", "Improving");

                alertSystem.addAlert(
                    MEDIUM,
                    TEAM_UPDATE,
                    "Your team scored 45 points this week!",
                    "User"
                );

                alertSystem.addAlert(
                    LOW,
                    SYSTEM_MESSAGE,
                    "Next race: Monaco GP in 3 days",
                    "System"
                );

                cout << "✅ Test alerts created!" << endl;
                break;
            }

            case 6: {
                cout << "Clear all alerts? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (confirm == 'y' || confirm == 'Y') {
                    alertSystem.clearAllAlerts();
                    cout << "✅ All alerts cleared!" << endl;
                }
                break;
            }

            case 7:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice!" << endl;
        }

        if (alertChoice != 7) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (alertChoice != 7);
    break;
}
            case 12:
                cout << "\n========================================" << endl;
                cout << "  Thanks for playing F1 Fantasy League!" << endl;
                cout << "========================================" << endl;
                cout << "\n📊 Session Summary:" << endl;
                cout << "Total Races Simulated: [track this]" << endl;
                cout << "Teams Created: " << leaderboard.getTotalUsers() << endl;
                cout << "Alerts Generated: " << alertSystem.getTotalAlertsCount() << endl;
                cout << "\nGoodbye! 🏁" << endl;
                return 0;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}

void displayMainMenu() {
    cout << "\n========================================" << endl;
    cout << "        F1 FANTASY LEAGUE 2024" << endl;
    cout << "========================================" << endl;
    cout << "1. Create New Fantasy Team" << endl;
    cout << "2. Driver Rankings (QuickSort)" << endl;
    cout << "3. Test QuickSort Performance" << endl;
    cout << "4. Driver Rankings (AVL Tree)" << endl;
    cout << "5. Update Driver Points" << endl;
    cout << "6. Search Drivers (Trie)" << endl;
    cout << "7. Simulate Race" << endl;
    cout << "8. Team Builder (Knapsack)" << endl;
    cout << "9. Leaderboard" << endl;
    cout << "10. Performance Analyzer - NEW!" << endl;
    cout << "11. Alert Center - NEW!" << endl;
    cout << "12. Exit" << endl;
    cout << "========================================" << endl;

    // Show unread alerts count if any
    // if (AlertSystem.hasAlerts()) {
    //     cout << "📬 You have " << AlertSystem.getUnreadCount()
    //          << " unread alert(s)!" << endl;
    // }
}

void displayAVLMenu(AVLTree& avlTree) {
    int choice;
    do {
        cout << "\n=== DRIVER RANKINGS - AVL TREE ===" << endl;
        cout << "1. View All Driver Rankings" << endl;
        cout << "2. View Top 5 Drivers" << endl;
        cout << "3. View Top 10 Drivers" << endl;
        cout << "4. Search for Specific Driver" << endl;
        cout << "5. View Tree Structure (Visualization)" << endl;
        cout << "6. Tree Statistics" << endl;
        cout << "7. Compare with QuickSort" << endl;
        cout << "8. Back to Main Menu" << endl;
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(choice) {
            case 1: {
                cout << "\n=== ALL DRIVERS (AVL Tree) ===" << endl;
                avlTree.displayRankings();
                break;
            }

            case 2: {
                vector<Driver> top5 = avlTree.getTopNDrivers(5);
                cout << "\n=== TOP 5 DRIVERS ===" << endl;
                cout << "Rank | Code | Name                | Points | Price" << endl;
                cout << "---------------------------------------------------" << endl;
                for (int i = 0; i < top5.size(); i++) {
                    cout << (i + 1) << ".   " << top5[i].getCode() << "     "
                         << top5[i].getName();
                    int nameLen = top5[i].getName().length();
                    for (int j = nameLen; j < 20; j++) cout << " ";
                    cout << top5[i].getPoints() << "      "
                         << top5[i].getPrice() << "M" << endl;
                }
                break;
            }

            case 3: {
                vector<Driver> top10 = avlTree.getTopNDrivers(10);
                cout << "\n=== TOP 10 DRIVERS ===" << endl;
                cout << "Rank | Code | Name                | Points | Price" << endl;
                cout << "---------------------------------------------------" << endl;
                for (int i = 0; i < top10.size(); i++) {
                    cout << (i + 1) << ".   " << top10[i].getCode() << "     "
                         << top10[i].getName();
                    int nameLen = top10[i].getName().length();
                    for (int j = nameLen; j < 20; j++) cout << " ";
                    cout << top10[i].getPoints() << "      "
                         << top10[i].getPrice() << "M" << endl;
                }
                break;
            }

            case 4: {
                string code;
                cout << "Enter driver code: ";
                getline(cin, code);

                Driver* found = avlTree.search(code);
                if (found != nullptr) {
                    cout << "\n=== DRIVER FOUND ===" << endl;
                    cout << "Code: " << found->getCode() << endl;
                    cout << "Name: " << found->getName() << endl;
                    cout << "Team: " << found->getTeam() << endl;
                    cout << "Points: " << found->getPoints() << endl;
                    cout << "Price: " << found->getPrice() << "M" << endl;
                } else {
                    cout << "Driver not found!" << endl;
                }
                break;
            }

            case 5: {
                avlTree.displayTreeStructure();
                break;
            }

            case 6: {
                cout << "\n=== AVL TREE STATISTICS ===" << endl;
                cout << "Total Drivers: " << avlTree.size() << endl;
                cout << "Tree Empty: " << (avlTree.isEmpty() ? "Yes" : "No") << endl;

                vector<Driver> all = avlTree.getDriversByPointsDescending();
                if (!all.empty()) {
                    cout << "Highest Points: " << all[0].getPoints()
                         << " (" << all[0].getName() << ")" << endl;
                    cout << "Lowest Points: " << all[all.size()-1].getPoints()
                         << " (" << all[all.size()-1].getName() << ")" << endl;
                }
                break;
            }

            case 7: {
                cout << "\n=== PERFORMANCE COMPARISON: AVL vs QuickSort ===" << endl;

                // Get drivers from AVL tree
                vector<Driver> avlDrivers = avlTree.getDriversByPointsDescending();

                // Time AVL traversal
                clock_t avlStart = clock();
                vector<Driver> avlResult = avlTree.getDriversByPointsDescending();
                clock_t avlEnd = clock();
                double avlTime = double(avlEnd - avlStart) / CLOCKS_PER_SEC;

                // Time QuickSort
                vector<Driver> quickDrivers = avlDrivers;
                clock_t quickStart = clock();
                SortingUtils::quickSortDriversByPoints(quickDrivers);
                clock_t quickEnd = clock();
                double quickTime = double(quickEnd - quickStart) / CLOCKS_PER_SEC;

                cout << "Dataset size: " << avlDrivers.size() << " drivers" << endl;
                cout << "\nAVL Tree Traversal Time: " << avlTime << " seconds" << endl;
                cout << "QuickSort Time: " << quickTime << " seconds" << endl;

                cout << "\nNote: AVL tree is already sorted during insertion," << endl;
                cout << "so retrieval is O(n) vs QuickSort's O(n log n)" << endl;

                // Verify both give same results
                bool same = true;
                for (int i = 0; i < avlResult.size(); i++) {
                    if (avlResult[i].getCode() != quickDrivers[i].getCode()) {
                        same = false;
                        break;
                    }
                }
                cout << "Results match: " << (same ? "YES" : "NO") << endl;
                break;
            }

            case 8:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice! Try again." << endl;
        }

        if (choice != 8) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 8);
}

void displaySortingMenu(vector<Driver>& drivers, vector<Constructors>& constructors) {
    int choice;
    do {
        cout << "\n=== DRIVER RANKINGS - QUICKSORT ===" << endl;
        cout << "1. Sort Drivers by Points (Highest First)" << endl;
        cout << "2. Sort Drivers by Price (Cheapest First)" << endl;
        cout << "3. View Top 5 Drivers by Points" << endl;
        cout << "4. View Top 5 Drivers by Price" << endl;
        cout << "5. View All Drivers Sorted by Points" << endl;
        cout << "6. View All Drivers Sorted by Price" << endl;
        cout << "7. Compare Points vs Price Rankings" << endl;
        cout << "8. Back to Main Menu" << endl;
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<Driver> sortedDrivers;

        switch(choice) {
            case 1: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPoints(sortedDrivers);
                cout << "\n=== DRIVERS SORTED BY POINTS (QUICKSORT) ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers);
                break;
            }

            case 2: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPrice(sortedDrivers);
                cout << "\n=== DRIVERS SORTED BY PRICE (QUICKSORT) ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers);
                break;
            }

            case 3: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPoints(sortedDrivers);
                cout << "\n=== TOP 5 DRIVERS BY POINTS ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers, 5);
                break;
            }

            case 4: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPrice(sortedDrivers);
                cout << "\n=== TOP 5 DRIVERS BY PRICE ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers, 5);
                break;
            }

            case 5: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPoints(sortedDrivers);
                cout << "\n=== ALL DRIVERS BY POINTS ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers, drivers.size());
                break;
            }

            case 6: {
                sortedDrivers = drivers;
                SortingUtils::quickSortDriversByPrice(sortedDrivers);
                cout << "\n=== ALL DRIVERS BY PRICE ===" << endl;
                SortingUtils::displayTopDrivers(sortedDrivers, drivers.size());
                break;
            }

            case 7: {
                cout << "\n=== COMPARISON: POINTS VS PRICE ===" << endl;

                vector<Driver> byPoints = drivers;
                SortingUtils::quickSortDriversByPoints(byPoints);

                vector<Driver> byPrice = drivers;
                SortingUtils::quickSortDriversByPrice(byPrice);

                cout << "\nTop 3 by Points vs Top 3 by Price:" << endl;
                cout << "-----------------------------------" << endl;
                cout << "By Points\t\tBy Price" << endl;
                cout << "---------\t\t--------" << endl;

                for (int i = 0; i < 3 && i < drivers.size(); i++) {
                    cout << i+1 << ". " << byPoints[i].getName()
                         << " (" << byPoints[i].getPoints() << " pts)\t"
                         << i+1 << ". " << byPrice[i].getName()
                         << " (" << byPrice[i].getPrice() << "M)" << endl;
                }

                cout << "\n=== BEST VALUE DRIVERS ===" << endl;
                cout << "(High points relative to price)" << endl;

                vector<pair<float, Driver>> valueDrivers;
                for (Driver& d : drivers) {
                    if (d.getPrice() > 0) {
                        float valueScore = d.getPoints() / d.getPrice();
                        valueDrivers.push_back({valueScore, d});
                    }
                }

                sort(valueDrivers.begin(), valueDrivers.end(),
                     [](const pair<float, Driver>& a, const pair<float, Driver>& b) {
                         return a.first > b.first;
                     });

                cout << "Rank | Driver\t\t| Points | Price | Value Score" << endl;
                cout << "-------------------------------------------------" << endl;
                for (int i = 0; i < min(5, (int)valueDrivers.size()); i++) {
                    cout << i+1 << ".   " << valueDrivers[i].second.getName();

                    int nameLen = valueDrivers[i].second.getName().length();
                    for (int j = nameLen; j < 15; j++) cout << " ";

                    cout << valueDrivers[i].second.getPoints() << "\t  "
                         << valueDrivers[i].second.getPrice() << "M\t  "
                         << valueDrivers[i].first << endl;
                }
                break;
            }

            case 8:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice! Try again." << endl;
        }

        if (choice != 8) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 8);
}

// ============ HELPER FUNCTIONS ============

vector<Driver> LoadDrivers(string filename) {
    vector<Driver> drivers;
    ifstream file(filename);
    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string code, name, team;
        int avgFinishes, wins, podiums;
        float price;

        getline(ss, code, ',');
        getline(ss, name, ',');
        getline(ss, team, ',');
        ss >> price; ss.ignore();
        ss >> avgFinishes; ss.ignore();
        ss >> wins; ss.ignore();
        ss >> podiums;

        drivers.push_back(Driver(code,name,team,0,price,avgFinishes,wins,podiums));
    }
    return drivers;
}

vector<Constructors> LoadConstructors(string filename) {
    vector<Constructors> constructors;
    ifstream file(filename);
    string line;
    getline(file,line);

    while (getline(file,line)) {
        stringstream ss(line);
        string code, teamname;
        float price;

        getline(ss,code,',');
        getline(ss,teamname, ',');
        ss >> price; ss.ignore();

        constructors.push_back(Constructors(code,teamname,price));
    }
    return constructors;
}

Driver SelectDriver(const vector<Driver> &drivers) {
    for ( Driver driver : drivers ) {
        cout << driver.getCode() << " " << driver.getName() << " " << driver.getTeam() << " " << driver.getPrice() << endl;
    }

    string cd;
    while (true) {
        cout << "Enter Driver code: ";
        getline(cin, cd);
        for (Driver drive : drivers) {
            if (cd == drive.getCode()) {
                return drive;
            }
        }
        cout << "Invalid Code" << endl;
    }
}

Constructors SelectConstructor(const vector<Constructors> &constructors) {
    for (Constructors c : constructors) {
        cout << c.getCode() << " " << c.getTeamName() << " " << c.getPrice() << endl;
    }

    string cd;
    while (true) {
        cout << "Enter Constructor code: ";
        getline(cin,cd);
        for (Constructors c : constructors) {
            if (cd == c.getCode()) {
                return c;
            }
        }
        cout << "Invalid Code" << endl;
    }
}

bool StoreUserTeam(string filename, User user) {
    ofstream file(filename, ios::app);

    file << user.getUsername() << "," << user.getBudget() << "," << user.getTeam().getConstructor().getTeamName() << ",";
    vector<Driver> drivers = user.getTeam().getDrivers();
    for (Driver d : drivers) {
        file << d.getName() << ",";
    }
    file << user.getPoints();
    file << "\n";
    return true;
}