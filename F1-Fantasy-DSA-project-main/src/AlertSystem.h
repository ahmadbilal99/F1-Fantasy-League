//
// Alert System using Priority Queue
// Manages notifications by priority
//

#ifndef F1_FANTASY_DSA_ALERTSYSTEM_H
#define F1_FANTASY_DSA_ALERTSYSTEM_H

#include <string>
#include <queue>
#include <vector>
#include <ctime>
using namespace std;

enum AlertPriority {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

enum AlertType {
    DRIVER_INJURY,
    PRICE_CHANGE,
    FORM_CHANGE,
    RACE_RESULT,
    TEAM_UPDATE,
    SYSTEM_MESSAGE
};

struct Alert {
    AlertPriority priority;
    AlertType type;
    string message;
    string driverOrTeam;
    time_t timestamp;
    bool read;

    Alert(AlertPriority p, AlertType t, const string& msg, const string& entity = "")
        : priority(p), type(t), message(msg), driverOrTeam(entity), read(false) {
        timestamp = time(0);
    }
};

// Comparator for priority queue (higher priority = higher in queue)
struct CompareAlerts {
    bool operator()(const Alert& a, const Alert& b) {
        if (a.priority != b.priority) {
            return a.priority < b.priority;  // Max heap by priority
        }
        return a.timestamp > b.timestamp;  // Newer first if same priority
    }
};

class AlertSystem {
private:
    priority_queue<Alert, vector<Alert>, CompareAlerts> activeAlerts;
    vector<Alert> alertHistory;
    int maxHistorySize;

    // Helper functions
    string priorityToString(AlertPriority priority);
    string typeToString(AlertType type);
    string getIcon(AlertPriority priority);

public:
    AlertSystem(int historySize = 100);

    // Add alerts
    void addAlert(AlertPriority priority, AlertType type, const string& message, const string& entity = "");
    void addDriverInjury(const string& driverName, const string& details);
    void addPriceChange(const string& driverName, float oldPrice, float newPrice);
    void addFormAlert(const string& driverName, const string& trend);
    void addRaceResult(const string& raceName, const string& winner);

    // Get alerts
    Alert getNextAlert();
    vector<Alert> getTopNAlerts(int n);
    vector<Alert> getUnreadAlerts();

    // Management
    void markAsRead(Alert& alert);
    void clearReadAlerts();
    void clearAllAlerts();

    // Display
    void displayAlerts();
    void displayUnreadAlerts();
    void displayAlertHistory();
    void displayAlertsByPriority(AlertPriority priority);

    // Statistics
    int getUnreadCount() const;
    int getTotalAlertsCount() const;
    int getCriticalCount() const;

    // Utility
    bool hasAlerts() const;
};

#endif //F1_FANTASY_DSA_ALERTSYSTEM_H