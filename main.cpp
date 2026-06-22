#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

const string USER_FILE = "users.txt", BIN_FILE = "bins.txt", TRUCK_FILE = "trucks.txt";

void clearInput() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

string inputLine(const string& msg) {
    string value;
    cout << msg;
    getline(cin, value);
    return value;
}

vector<string> split(const string& line) {
    vector<string> parts;
    string part;
    stringstream ss(line);
    while (getline(ss, part, '|')) parts.push_back(part);
    return parts;
}

int toInt(const string& text) { stringstream ss(text); int value = 0; ss >> value; return value; }
double toDouble(const string& text) { stringstream ss(text); double value = 0; ss >> value; return value; }

double limitPercent(double value) {
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

string lowerText(string text) {
    for (int i = 0; i < (int)text.length(); i++) text[i] = (char)tolower((unsigned char)text[i]);
    return text;
}

string normalizeWasteType(string type) {
    type = lowerText(type);
    if (type == "recycle" || type == "recyclable") return "Recyclable";
    if (type == "organic" || type == "compost") return "Organic";
    if (type == "hazard" || type == "hazardous") return "Hazardous";
    return "Unknown";
}

bool validWasteType(const string& type) { return normalizeWasteType(type) != "Unknown"; }

void printBinHeader() {
    cout << left << setw(10) << "Bin ID" << setw(20) << "Location"
         << setw(15) << "Waste Type" << setw(15) << "Fill Level"
         << setw(15) << "Status" << '\n' << string(75, '-') << '\n';
}

void printTruckHeader() {
    cout << left << setw(10) << "Truck ID" << setw(25) << "Truck Type"
         << setw(15) << "Capacity" << setw(15) << "Load"
         << setw(25) << "Assigned Route" << '\n' << string(90, '-') << '\n';
}

void signUp() {
    clearInput();
    string username = inputLine("Enter Username: ");
    string password = inputLine("Enter Password: ");
    string role = lowerText(inputLine("Role (admin/user): "));

    if (role != "admin" && role != "user") {
        cout << "Invalid role. Use admin or user.\n";
        return;
    }

    ofstream file(USER_FILE.c_str(), ios::app);
    file << username << '|' << password << '|' << role << '\n';
    cout << "Account created successfully.\n";
}

string login() {
    clearInput();
    string username = inputLine("Username: ");
    string password = inputLine("Password: ");
    ifstream file(USER_FILE.c_str());
    string line;

    while (getline(file, line)) {
        vector<string> user = split(line);
        if (user.size() == 3 && user[0] == username && user[1] == password) return user[2];
    }
    return "";
}

struct WasteInfo {
    string name;
    bool recyclable;
    int priority;

    WasteInfo(string n = "Unknown", bool r = false, int p = 0) {
        name = n;
        recyclable = r;
        priority = p;
    }
};

WasteInfo classifyWaste(const string& type) {
    string waste = normalizeWasteType(type);
    if (waste == "Recyclable") return WasteInfo("Recyclable", true, 2);
    if (waste == "Organic") return WasteInfo("Organic", false, 1);
    if (waste == "Hazardous") return WasteInfo("Hazardous", false, 3);
    return WasteInfo("Unknown", false, 0);
}

void saveWasteReport(const WasteInfo& info, const string& searchedText) {
    ofstream file("waste_report.txt", ios::app);
    if (info.name == "Unknown") file << "Invalid waste type searched: " << searchedText << '\n';
    else file << "Waste Type: " << info.name
              << " | Recycling Status: " << (info.recyclable ? "Recyclable" : "Not Recyclable")
              << " | Priority: " << info.priority << '\n';
}

class SmartBin {
    int id;
    string location, wasteType;
    double fillLevel;

public:
    SmartBin(int binId = 0, string loc = "Unknown", string type = "Unknown", double level = 0) {
        id = binId;
        location = loc;
        wasteType = normalizeWasteType(type);
        fillLevel = limitPercent(level);
    }

    int getId() const { return id; }
    string getLocation() const { return location; }
    string getWasteType() const { return wasteType; }
    double getFillLevel() const { return fillLevel; }
    bool isOverflowing() const { return fillLevel >= 90; }
    bool needsCollection() const { return fillLevel >= 80; }
    void addWaste(double amount) { fillLevel = limitPercent(fillLevel + amount); }
    void emptyBin() { fillLevel = 0; }

    void update(string newLocation, string newType, double newLevel) {
        location = newLocation;
        wasteType = normalizeWasteType(newType);
        fillLevel = limitPercent(newLevel);
    }

    void display() const {
        cout << left << setw(10) << id << setw(20) << location << setw(15) << wasteType
             << setw(15) << fillLevel << setw(15) << (isOverflowing() ? "Overflow" : "Normal") << '\n';
    }

    string toFileString() const {
        stringstream ss;
        ss << id << '|' << location << '|' << wasteType << '|' << fillLevel;
        return ss.str();
    }

    static SmartBin fromFileString(const string& line) {
        vector<string> d = split(line);
        return d.size() < 4 ? SmartBin() : SmartBin(toInt(d[0]), d[1], d[2], toDouble(d[3]));
    }
};

class Truck {
    int id;
    string type, assignedRoute;
    double capacity, currentLoad;

public:
    Truck(int truckId = 0, string truckType = "Unknown", double cap = 0) {
        id = truckId;
        type = truckType;
        capacity = cap;
        currentLoad = 0;
        assignedRoute = "Not Assigned";
    }

    int getId() const { return id; }
    string getType() const { return type; }
    string getRoute() const { return assignedRoute; }

    bool canCollect(const string& wasteType) const {
        string waste = normalizeWasteType(wasteType);
        return (type == "Recycling Truck" && waste == "Recyclable") ||
               (type == "Organic Compost Truck" && waste == "Organic") ||
               (type == "Hazardous Waste Hauler" && waste == "Hazardous");
    }

    bool collect(SmartBin& bin) {
        if (!canCollect(bin.getWasteType()) || currentLoad + bin.getFillLevel() > capacity) return false;
        currentLoad += bin.getFillLevel();
        assignedRoute = bin.getLocation();
        bin.emptyBin();
        return true;
    }

    void restore(double load, const string& route) {
        currentLoad = load;
        assignedRoute = route.empty() ? "Not Assigned" : route;
    }

    void display() const {
        cout << left << setw(10) << id << setw(25) << type << setw(15) << capacity
             << setw(15) << currentLoad << setw(25) << assignedRoute << '\n';
    }

    string toFileString() const {
        stringstream ss;
        ss << id << '|' << type << '|' << capacity << '|' << currentLoad << '|' << assignedRoute;
        return ss.str();
    }

    static Truck fromFileString(const string& line) {
        vector<string> d = split(line);
        if (d.size() < 5) return Truck();
        Truck truck(toInt(d[0]), d[1], toDouble(d[2]));
        truck.restore(toDouble(d[3]), d[4]);
        return truck;
    }
};

string truckTypeFromChoice(int choice) {
    if (choice == 1) return "Recycling Truck";
    if (choice == 2) return "Organic Compost Truck";
    if (choice == 3) return "Hazardous Waste Hauler";
    return "";
}

class WasteManagementSystem {
    vector<SmartBin> bins;
    vector<Truck> trucks;

    int findBinIndex(int id) const {
        for (int i = 0; i < (int)bins.size(); i++) if (bins[i].getId() == id) return i;
        return -1;
    }

    int findTruckIndex(int id) const {
        for (int i = 0; i < (int)trucks.size(); i++) if (trucks[i].getId() == id) return i;
        return -1;
    }

public:
    WasteManagementSystem() { loadBins(); loadTrucks(); }
    ~WasteManagementSystem() { saveBins(); saveTrucks(); }

    void loadBins() {
        bins.clear();
        ifstream file(BIN_FILE.c_str());
        string line;
        while (getline(file, line)) if (!line.empty()) bins.push_back(SmartBin::fromFileString(line));
    }

    void saveBins() const {
        ofstream file(BIN_FILE.c_str());
        for (int i = 0; i < (int)bins.size(); i++) file << bins[i].toFileString() << '\n';
    }

    void loadTrucks() {
        trucks.clear();
        ifstream file(TRUCK_FILE.c_str());
        string line;
        while (getline(file, line)) if (!line.empty()) trucks.push_back(Truck::fromFileString(line));
    }

    void saveTrucks() const {
        ofstream file(TRUCK_FILE.c_str());
        for (int i = 0; i < (int)trucks.size(); i++) file << trucks[i].toFileString() << '\n';
    }

    void addSmartBin() {
        int id;
        double level;
        cout << "Enter Bin ID: ";
        cin >> id;

        if (findBinIndex(id) != -1) {
            cout << "Bin ID already exists.\n";
            return;
        }

        clearInput();
        string location = inputLine("Enter Bin Location: ");
        string type = inputLine("Enter Waste Type (Recyclable / Organic / Hazardous): ");

        if (!validWasteType(type)) {
            cout << "Invalid waste type.\n";
            return;
        }

        cout << "Enter Current Fill Level (0-100): ";
        cin >> level;
        bins.push_back(SmartBin(id, location, type, level));
        saveBins();
        cout << "Smart bin added successfully.\n";
    }

    void viewSmartBins() const {
        cout << "\n========== SMART BINS ==========\n";
        if (bins.empty()) {
            cout << "No smart bins available.\n";
            return;
        }
        printBinHeader();
        for (int i = 0; i < (int)bins.size(); i++) bins[i].display();
    }

    void addWasteToBin() {
        int id;
        double amount;
        cout << "Enter Bin ID: ";
        cin >> id;

        int index = findBinIndex(id);
        if (index == -1) {
            cout << "Bin not found.\n";
            return;
        }

        cout << "Enter waste amount to add: ";
        cin >> amount;
        bins[index].addWaste(amount);
        saveBins();
        cout << "Waste added successfully.\n";
    }

    void searchBin() const {
        int id;
        cout << "Enter Bin ID to search: ";
        cin >> id;

        int index = findBinIndex(id);
        if (index == -1) cout << "Bin not found.\n";
        else { printBinHeader(); bins[index].display(); }
    }

    void updateBinStatus() {
        int id;
        double level;
        cout << "Enter Bin ID to update: ";
        cin >> id;

        int index = findBinIndex(id);
        if (index == -1) {
            cout << "Bin not found.\n";
            return;
        }

        clearInput();
        string location = inputLine("Enter New Location: ");
        string type = inputLine("Enter New Waste Type (Recyclable / Organic / Hazardous): ");

        if (!validWasteType(type)) {
            cout << "Invalid waste type.\n";
            return;
        }

        cout << "Enter New Fill Level (0-100): ";
        cin >> level;
        bins[index].update(location, type, level);
        saveBins();
        cout << "Bin updated successfully.\n";
    }

    void addGarbageTruck() {
        int id, choice;
        double capacity;
        cout << "Enter Truck ID: ";
        cin >> id;

        if (findTruckIndex(id) != -1) {
            cout << "Truck ID already exists.\n";
            return;
        }

        cout << "Select Truck Type:\n1. Recycling Truck\n2. Organic Compost Truck\n3. Hazardous Waste Hauler\nEnter Choice: ";
        cin >> choice;

        string type = truckTypeFromChoice(choice);
        if (type.empty()) {
            cout << "Invalid truck type.\n";
            return;
        }

        cout << "Enter Truck Capacity: ";
        cin >> capacity;
        trucks.push_back(Truck(id, type, capacity));
        saveTrucks();
        cout << "Garbage truck added successfully.\n";
    }

    void viewGarbageTrucks() const {
        cout << "\n========== GARBAGE TRUCKS ==========\n";
        if (trucks.empty()) {
            cout << "No garbage trucks available.\n";
            return;
        }
        printTruckHeader();
        for (int i = 0; i < (int)trucks.size(); i++) trucks[i].display();
    }

    void classifyWasteMenu() {
        clearInput();
        string type = inputLine("Enter Waste Type to classify: ");
        WasteInfo info = classifyWaste(type);

        if (info.name == "Unknown") {
            cout << "Invalid waste type.\n";
            saveWasteReport(info, type);
            return;
        }

        cout << "\nWaste Category: " << info.name << '\n';
        cout << "Recycling Status: " << (info.recyclable ? "Recyclable" : "Not Recyclable") << '\n';
        cout << "Collection Priority: " << info.priority << '\n';
        saveWasteReport(info, type);
        cout << "Waste report saved in waste_report.txt\n";
    }

    void showCollectionPlan() const {
        bool found = false;
        cout << "\n========== DAILY COLLECTION PLAN ==========\n";
        printBinHeader();

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].needsCollection()) {
                bins[i].display();
                found = true;
            }
        }
        if (!found) cout << "No bins need collection today.\n";
    }

    void optimizeDailyRoutes() {
        bool anyCollected = false;
        ofstream file("collections.txt", ios::app);
        showCollectionPlan();
        cout << "\n========== OPTIMIZED ROUTE RESULT ==========\n";

        for (int i = 0; i < (int)bins.size(); i++) {
            if (!bins[i].needsCollection()) continue;

            bool collected = false;
            for (int j = 0; j < (int)trucks.size(); j++) {
                string location = bins[i].getLocation(), wasteType = bins[i].getWasteType();

                if (trucks[j].collect(bins[i])) {
                    cout << "Bin " << bins[i].getId() << " at " << location
                         << " collected by Truck " << trucks[j].getId()
                         << " (" << trucks[j].getType() << ")\n";

                    file << "Bin ID: " << bins[i].getId()
                         << " | Location: " << location
                         << " | Waste Type: " << wasteType
                         << " | Collected By Truck: " << trucks[j].getId()
                         << " | Truck Type: " << trucks[j].getType() << '\n';

                    collected = anyCollected = true;
                    break;
                }
            }

            if (!collected) cout << "No suitable truck available for Bin " << bins[i].getId()
                                  << " (" << bins[i].getWasteType() << ")\n";
        }

        if (!anyCollected) cout << "No collection completed. Either bins are not full or trucks are not suitable.\n";
        saveBins();
        saveTrucks();
    }

    void generateSystemReport() const {
        int overflow = 0, needCollection = 0, recyclable = 0, organic = 0, hazardous = 0;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].isOverflowing()) overflow++;
            if (bins[i].needsCollection()) needCollection++;
            if (bins[i].getWasteType() == "Recyclable") recyclable++;
            else if (bins[i].getWasteType() == "Organic") organic++;
            else if (bins[i].getWasteType() == "Hazardous") hazardous++;
        }

        ofstream file("system_report.txt");
        file << "========== WASTE MANAGEMENT SYSTEM REPORT ==========\n"
             << "Total Smart Bins: " << bins.size() << '\n'
             << "Total Garbage Trucks: " << trucks.size() << '\n'
             << "Overflowing Bins: " << overflow << '\n'
             << "Bins Needing Collection: " << needCollection << '\n'
             << "Recyclable Bins: " << recyclable << '\n'
             << "Organic Bins: " << organic << '\n'
             << "Hazardous Bins: " << hazardous << '\n'
             << "Fuel Saving Idea: Only bins with 80% or more fill level are collected.\n";

        cout << "System report generated successfully in system_report.txt\n";
    }

    void userMenu() {
        int choice;
        do {
            cout << "\n====================================\n"
                 << "          USER DASHBOARD\n"
                 << "====================================\n"
                 << "1. View Smart Bins\n2. Search Bin\n3. Classify Waste\n"
                 << "4. Generate System Report\n0. Logout\nEnter Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: viewSmartBins(); break;
                case 2: searchBin(); break;
                case 3: classifyWasteMenu(); break;
                case 4: generateSystemReport(); break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 0);
    }

    void adminMenu() {
        int choice;
        do {
            cout << "\n====================================================\n"
                 << " AUTOMATED WASTE MANAGEMENT & RECYCLING OPTIMIZER\n"
                 << "====================================================\n"
                 << "1. Add Smart Bin\n2. View Smart Bins\n3. Add Waste to Bin\n"
                 << "4. Search Bin\n5. Update Bin Status\n6. Add Garbage Truck\n"
                 << "7. View Garbage Trucks\n8. Classify Waste\n9. Optimize Daily Routes\n"
                 << "10. Generate System Report\n0. Logout\nEnter Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addSmartBin(); break;
                case 2: viewSmartBins(); break;
                case 3: addWasteToBin(); break;
                case 4: searchBin(); break;
                case 5: updateBinStatus(); break;
                case 6: addGarbageTruck(); break;
                case 7: viewGarbageTrucks(); break;
                case 8: classifyWasteMenu(); break;
                case 9: optimizeDailyRoutes(); break;
                case 10: generateSystemReport(); break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    WasteManagementSystem system;
    int choice;

    do {
        cout << "\n=====================================\n"
             << "      WASTE MANAGEMENT SYSTEM\n"
             << "=====================================\n"
             << "1. Sign Up\n2. Login\n0. Exit\nEnter Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                signUp();
                break;
            case 2: {
                string role = login();
                if (role == "admin") {
                    cout << "\nLogin Successful as Admin!\n";
                    system.adminMenu();
                } else if (role == "user") {
                    cout << "\nLogin Successful as User!\n";
                    system.userMenu();
                } else {
                    cout << "\nInvalid Username or Password!\n";
                }
                break;
            }
            case 0:
                cout << "Saving data and exiting program...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
