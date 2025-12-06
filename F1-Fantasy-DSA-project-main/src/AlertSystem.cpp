//
// Alert System Implementation
//

#include "AlertSystem.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ============ CONSTRUCTOR ============

AlertSystem::AlertSystem(int historySize) : maxHistorySize(historySize) {}

// ============ HELPER FUNCTIONS ============

string AlertSystem::priorityToString(AlertPriority priority) {
    switch (priority) {
        case LOW: return "LOW";
        case MEDIUM: return "MEDIUM";
        case HIGH: return "HIGH";
        case CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

string AlertSystem::typeToString(AlertType type) {
    switch (type) {
        case DRIVER_INJURY: return "Injury";
        case PRICE_CHANGE: return "Price";
        case FORM_CHANGE: return "Form";
        case RACE_RESULT: return "Race";
        case TEAM_UPDATE: return "Team";
        case SYSTEM_MESSAGE: return "System";
        default: return "Other";
    }
}

string AlertSystem::getIcon(AlertPriority priority) {
    switch (priority) {
        case LOW: return "ℹ️";
        case MEDIUM: return "⚠️";
        case HIGH: return "🔥";
        case CRITICAL: return "🚨";
        default: return "📢";
    }
}

// ============ ADD ALERTS ============

void AlertSystem::addAlert(AlertPriority priority, AlertType type, const string& message, const string& entity) {
    Alert alert(priority, type, message, entity);
    activeAlerts.push(alert);

    // Add to history
    alertHistory.push_back(alert);
    if (alertHistory.size() > maxHistorySize) {
        alertHistory.erase(alertHistory.begin());
    }
}

void AlertSystem::addDriverInjury(const string& driverName, const string& details) {
    string message = driverName + " - " + details;
    addAlert(CRITICAL, DRIVER_INJURY, message, driverName);
}

void AlertSystem::addPriceChange(const string& driverName, float oldPrice, float newPrice) {
    stringstream ss;
    ss << driverName << " price changed: " << oldPrice << "M → " << newPrice << "M";

    AlertPriority priority = (abs(newPrice - oldPrice) > 2.0) ? HIGH : MEDIUM;
    addAlert(priority, PRICE_CHANGE, ss.str(), driverName);
}

void AlertSystem::addFormAlert(const string& driverName, const string& trend) {
    string message = driverName + " form: " + trend;
    AlertPriority priority = (trend == "Improving") ? HIGH : MEDIUM;
    addAlert(priority, FORM_CHANGE, message, driverName);
}

void AlertSystem::addRaceResult(const string& raceName, const string& winner) {
    string message = raceName + " - Winner: " + winner;
    addAlert(MEDIUM, RACE_RESULT, message, winner);
}

// ============ GET ALERTS ============

Alert AlertSystem::getNextAlert() {
    if (activeAlerts.empty()) {
        return Alert(LOW, SYSTEM_MESSAGE, "No alerts");
    }

    Alert alert = activeAlerts.top();
    activeAlerts.pop();
    return alert;
}

vector<Alert> AlertSystem::getTopNAlerts(int n) {
    vector<Alert> topAlerts;
    priority_queue<Alert, vector<Alert>, CompareAlerts> tempQueue = activeAlerts;

    for (int i = 0; i < n && !tempQueue.empty(); i++) {
        topAlerts.push_back(tempQueue.top());
        tempQueue.pop();
    }

    return topAlerts;
}

vector<Alert> AlertSystem::getUnreadAlerts() {
    vector<Alert> unread;
    priority_queue<Alert, vector<Alert>, CompareAlerts> tempQueue = activeAlerts;

    while (!tempQueue.empty()) {
        Alert alert = tempQueue.top();
        tempQueue.pop();
        if (!alert.read) {
            unread.push_back(alert);
        }
    }

    return unread;
}

// ============ MANAGEMENT ============

void AlertSystem::markAsRead(Alert& alert) {
    alert.read = true;
}

void AlertSystem::clearReadAlerts() {
    priority_queue<Alert, vector<Alert>, CompareAlerts> newQueue;

    while (!activeAlerts.empty()) {
        Alert alert = activeAlerts.top();
        activeAlerts.pop();
        if (!alert.read) {
            newQueue.push(alert);
        }
    }

    activeAlerts = newQueue;
}

void AlertSystem::clearAllAlerts() {
    while (!activeAlerts.empty()) {
        activeAlerts.pop();
    }
}

// ============ DISPLAY ============

void AlertSystem::displayAlerts() {
    if (activeAlerts.empty()) {
        cout << "No active alerts!" << endl;
        return;
    }

    vector<Alert> alerts = getTopNAlerts(activeAlerts.size());

    cout << "\n=== ACTIVE ALERTS ===" << endl;
    cout << "Total: " << alerts.size() << " | Unread: " << getUnreadCount() << endl;
    cout << "\nPriority | Type     | Message" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (const Alert& alert : alerts) {
        cout << getIcon(alert.priority) << " "
             << setw(8) << left << priorityToString(alert.priority) << " | "
             << setw(8) << typeToString(alert.type) << " | "
             << alert.message;

        if (!alert.read) {
            cout << " [NEW]";
        }
        cout << endl;
    }
    cout << endl;
}

void AlertSystem::displayUnreadAlerts() {
    vector<Alert> unread = getUnreadAlerts();

    if (unread.empty()) {
        cout << "No unread alerts!" << endl;
        return;
    }

    cout << "\n=== UNREAD ALERTS (" << unread.size() << ") ===" << endl;
    cout << "Priority | Type     | Message" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (const Alert& alert : unread) {
        cout << getIcon(alert.priority) << " "
             << setw(8) << left << priorityToString(alert.priority) << " | "
             << setw(8) << typeToString(alert.type) << " | "
             << alert.message << endl;
    }
    cout << endl;
}

void AlertSystem::displayAlertHistory() {
    if (alertHistory.empty()) {
        cout << "No alert history!" << endl;
        return;
    }

    cout << "\n=== ALERT HISTORY (Last " << alertHistory.size() << ") ===" << endl;
    cout << "Priority | Type     | Message" << endl;
    cout << "-------------------------------------------------------------" << endl;

    // Show most recent first
    for (auto it = alertHistory.rbegin(); it != alertHistory.rend(); ++it) {
        cout << getIcon(it->priority) << " "
             << setw(8) << left << priorityToString(it->priority) << " | "
             << setw(8) << typeToString(it->type) << " | "
             << it->message << endl;
    }
    cout << endl;
}

void AlertSystem::displayAlertsByPriority(AlertPriority priority) {
    vector<Alert> filtered;
    priority_queue<Alert, vector<Alert>, CompareAlerts> tempQueue = activeAlerts;

    while (!tempQueue.empty()) {
        Alert alert = tempQueue.top();
        tempQueue.pop();
        if (alert.priority == priority) {
            filtered.push_back(alert);
        }
    }

    if (filtered.empty()) {
        cout << "No " << priorityToString(priority) << " priority alerts!" << endl;
        return;
    }

    cout << "\n=== " << priorityToString(priority) << " PRIORITY ALERTS ===" << endl;
    cout << "Type     | Message" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (const Alert& alert : filtered) {
        cout << setw(8) << left << typeToString(alert.type) << " | "
             << alert.message << endl;
    }
    cout << endl;
}

// ============ STATISTICS ============

int AlertSystem::getUnreadCount() const {
    int count = 0;
    priority_queue<Alert, vector<Alert>, CompareAlerts> tempQueue = activeAlerts;

    while (!tempQueue.empty()) {
        if (!tempQueue.top().read) {
            count++;
        }
        tempQueue.pop();
    }

    return count;
}

int AlertSystem::getTotalAlertsCount() const {
    return activeAlerts.size();
}

int AlertSystem::getCriticalCount() const {
    int count = 0;
    priority_queue<Alert, vector<Alert>, CompareAlerts> tempQueue = activeAlerts;

    while (!tempQueue.empty()) {
        if (tempQueue.top().priority == CRITICAL) {
            count++;
        }
        tempQueue.pop();
    }

    return count;
}

// ============ UTILITY ============

bool AlertSystem::hasAlerts() const {
    return !activeAlerts.empty();
}