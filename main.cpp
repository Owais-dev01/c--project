#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <cctype>
using namespace std;

// ================= Helper Functions =================
int stringToInt(string s) {
    stringstream ss(s);
    int value = 0;
    ss >> value;
    return value;
}

double stringToDouble(string s) {
    stringstream ss(s);
    double value = 0;
    ss >> value;
    return value;
}

string toLowerCase(string text) {
    for (int i = 0; i < (int)text.length(); i++) {
        text[i] = (char)tolower(text[i]);
    }
    return text;
}

string normalizeWasteType(string wasteType) {
    wasteType = toLowerCase(wasteType);
    if (wasteType == "recycle" || wasteType == "recyclable") return "Recyclable";
    if (wasteType == "organic" || wasteType == "compost") return "Organic";
    if (wasteType == "hazardous" || wasteType == "hazard") return "Hazardous";
    return "Unknown";
}

// ================= Abstraction + Polymorphism =================
class WasteCategory {
public:
    virtual string getCategoryName() = 0;
    virtual bool isRecyclable() = 0;
    virtual int getPriority() = 0;
    virtual ~WasteCategory() {}
};

class RecyclableWaste : public WasteCategory {
public:
    string getCategoryName() { return "Recyclable"; }
    bool isRecyclable() { return true; }
    int getPriority() { return 2; }
};

class OrganicWaste : public WasteCategory {
public:
    string getCategoryName() { return "Organic"; }
    bool isRecyclable() { return false; }
    int getPriority() { return 1; }
};

class HazardousWaste : public WasteCategory {
public:
    string getCategoryName() { return "Hazardous"; }
    bool isRecyclable() { return false; }
    int getPriority() { return 3; }
};

class WasteClassifier {
public:
    static WasteCategory* classifyWaste(string wasteType) {
        wasteType = normalizeWasteType(wasteType);

        if (wasteType == "Recyclable") return new RecyclableWaste();
        if (wasteType == "Organic") return new OrganicWaste();
        if (wasteType == "Hazardous") return new HazardousWaste();

        return NULL;
    }

    static bool validateWasteType(string wasteType) {
        wasteType = normalizeWasteType(wasteType);
        return wasteType == "Recyclable" || wasteType == "Organic" || wasteType == "Hazardous";
    }

    static void generateWasteReport(string wasteType) {
        WasteCategory* category = classifyWaste(wasteType);

        ofstream file("waste_report.txt", ios::app);
        if (file.is_open()) {
            if (category != NULL) {
                file << "Waste Type: " << category->getCategoryName()
                     << " | Recycling Status: " << (category->isRecyclable() ? "Recyclable" : "Not Recyclable")
                     << " | Priority: " << category->getPriority() << endl;
            } else {
                file << "Invalid waste type searched: " << wasteType << endl;
            }
            file.close();
        }

        delete category;
    }
};

// ================= Smart Bin Class =================
class SmartBin {
private:
    int binId;
    string location;
    string wasteType;
    double fillLevel;
    bool overflowStatus;

public:
    SmartBin() {
        binId = 0;
        location = "Unknown";
        wasteType = "Unknown";
        fillLevel = 0;
        overflowStatus = false;
    }

    SmartBin(int id, string loc, string type, double level) {
        binId = id;
        location = loc;
        wasteType = normalizeWasteType(type);
        fillLevel = level;
        overflowStatus = detectOverflow();
    }

    ~SmartBin() {
        // Destructor included to demonstrate OOP concept
    }

    int getBinId() { return binId; }
    string getLocation() { return location; }
    string getWasteType() { return wasteType; }
    double getFillLevel() { return fillLevel; }
    bool getOverflowStatus() { return overflowStatus; }

    void setLocation(string loc) { location = loc; }
    void setWasteType(string type) { wasteType = normalizeWasteType(type); }
    void setFillLevel(double level) {
        fillLevel = level;
        if (fillLevel > 100) fillLevel = 100;
        if (fillLevel < 0) fillLevel = 0;
        overflowStatus = detectOverflow();
    }

    // Function Overloading
    void addWaste(int amount) {
        fillLevel += amount;
        if (fillLevel > 100) fillLevel = 100;
        overflowStatus = detectOverflow();
    }

    void addWaste(double amount) {
        fillLevel += amount;
        if (fillLevel > 100) fillLevel = 100;
        overflowStatus = detectOverflow();
    }

    bool detectOverflow() {
        return fillLevel >= 90;
    }

    bool needsCollection() {
        return fillLevel >= 80;
    }

    void emptyBin() {
        fillLevel = 0;
        overflowStatus = false;
    }

    void displayBinInformation() {
        cout << left << setw(10) << binId
             << setw(20) << location
             << setw(15) << wasteType
             << setw(15) << fillLevel
             << setw(15) << (overflowStatus ? "Overflow" : "Normal") << endl;
    }

    string toFileString() {
        stringstream ss;
        ss << binId << "|" << location << "|" << wasteType << "|" << fillLevel << "|" << overflowStatus;
        return ss.str();
    }

    static SmartBin fromFileString(string line) {
        string id, loc, type, level, overflow;
        stringstream ss(line);

        getline(ss, id, '|');
        getline(ss, loc, '|');
        getline(ss, type, '|');
        getline(ss, level, '|');
        getline(ss, overflow, '|');

        SmartBin bin(stringToInt(id), loc, type, stringToDouble(level));
        return bin;
    }
};

// ================= Garbage Truck Classes =================
class GarbageTruck {
protected:
    int truckId;
    string truckType;
    double capacity;
    double currentLoad;
    string assignedRoute;

public:
    GarbageTruck(int id, string type, double cap) {
        truckId = id;
        truckType = type;
        capacity = cap;
        currentLoad = 0;
        assignedRoute = "Not Assigned";
    }

    virtual ~GarbageTruck() {
        // Virtual destructor included for base class
    }

    int getTruckId() { return truckId; }
    string getTruckType() { return truckType; }
    double getCapacity() { return capacity; }
    double getCurrentLoad() { return currentLoad; }
    string getAssignedRoute() { return assignedRoute; }

    void setAssignedRoute(string route) { assignedRoute = route; }

    virtual bool canCollect(string wasteType) = 0;
    virtual string getTruckRole() = 0;

    virtual bool collectWaste(SmartBin &bin) {
        if (!canCollect(bin.getWasteType())) {
            return false;
        }

        double wasteAmount = bin.getFillLevel();
        if (currentLoad + wasteAmount > capacity) {
            return false;
        }

        currentLoad += wasteAmount;
        assignedRoute = bin.getLocation();
        bin.emptyBin();
        return true;
    }

    virtual void displayTruck() {
        cout << left << setw(10) << truckId
             << setw(25) << truckType
             << setw(15) << capacity
             << setw(15) << currentLoad
             << setw(25) << assignedRoute << endl;
    }

    string toFileString() {
        stringstream ss;
        ss << truckId << "|" << truckType << "|" << capacity << "|" << currentLoad << "|" << assignedRoute;
        return ss.str();
    }
};

class RecyclingTruck : public GarbageTruck {
public:
    RecyclingTruck(int id, double cap) : GarbageTruck(id, "Recycling Truck", cap) {}

    bool canCollect(string wasteType) {
        return normalizeWasteType(wasteType) == "Recyclable";
    }

    string getTruckRole() {
        return "Collects recyclable waste";
    }
};

class OrganicCompostTruck : public GarbageTruck {
public:
    OrganicCompostTruck(int id, double cap) : GarbageTruck(id, "Organic Compost Truck", cap) {}

    bool canCollect(string wasteType) {
        return normalizeWasteType(wasteType) == "Organic";
    }

    string getTruckRole() {
        return "Collects organic waste";
    }
};

class HazardousWasteHauler : public GarbageTruck {
public:
    HazardousWasteHauler(int id, double cap) : GarbageTruck(id, "Hazardous Waste Hauler", cap) {}

    bool canCollect(string wasteType) {
        return normalizeWasteType(wasteType) == "Hazardous";
    }

    string getTruckRole() {
        return "Collects hazardous waste";
    }
};

// ================= Route Optimization Module =================
class RouteOptimizer {
public:
    static void displayDailyCollectionPlan(vector<SmartBin> &bins) {
        cout << "\n========== DAILY COLLECTION PLAN ==========" << endl;
        cout << left << setw(10) << "Bin ID" << setw(20) << "Location" << setw(15) << "Waste Type" << setw(15) << "Fill Level" << setw(15) << "Status" << endl;
        cout << "--------------------------------------------------------------------------" << endl;

        bool found = false;
        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].needsCollection()) {
                bins[i].displayBinInformation();
                found = true;
            }
        }

        if (!found) {
            cout << "No bins need collection today." << endl;
        }
    }

    static void generateOptimizedRoutes(vector<SmartBin> &bins, vector<GarbageTruck*> &trucks) {
        ofstream collectionFile("collections.txt", ios::app);

        cout << "\n========== OPTIMIZED ROUTE RESULT ==========" << endl;

        bool anyCollected = false;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].needsCollection()) {
                bool collected = false;

                for (int j = 0; j < (int)trucks.size(); j++) {
                    if (trucks[j]->canCollect(bins[i].getWasteType())) {
                        if (trucks[j]->collectWaste(bins[i])) {
                            cout << "Bin " << bins[i].getBinId()
                                 << " at " << bins[i].getLocation()
                                 << " collected by Truck " << trucks[j]->getTruckId()
                                 << " (" << trucks[j]->getTruckType() << ")" << endl;

                            if (collectionFile.is_open()) {
                                collectionFile << "Bin ID: " << bins[i].getBinId()
                                               << " | Location: " << bins[i].getLocation()
                                               << " | Waste Type: " << bins[i].getWasteType()
                                               << " | Collected By Truck: " << trucks[j]->getTruckId()
                                               << " | Truck Type: " << trucks[j]->getTruckType() << endl;
                            }

                            collected = true;
                            anyCollected = true;
                            break;
                        }
                    }
                }

                if (!collected) {
                    cout << "No suitable truck available for Bin " << bins[i].getBinId()
                         << " (" << bins[i].getWasteType() << ")" << endl;
                }
            }
        }

        if (!anyCollected) {
            cout << "No collection completed. Either no bins are full or no suitable trucks exist." << endl;
        }

        if (collectionFile.is_open()) {
            collectionFile.close();
        }
    }
};

// ================= Main Waste Management System =================
class WasteManagementSystem {
private:
    vector<SmartBin> bins;
    vector<GarbageTruck*> trucks;

public:
    WasteManagementSystem() {
        loadBinsFromFile();
        loadTrucksFromFile();
    }

    ~WasteManagementSystem() {
        saveBinsToFile();
        saveTrucksToFile();

        for (int i = 0; i < (int)trucks.size(); i++) {
            delete trucks[i];
        }
    }

    void loadBinsFromFile() {
        bins.clear();
        ifstream file("bins.txt");
        string line;

        while (getline(file, line)) {
            if (line != "") {
                bins.push_back(SmartBin::fromFileString(line));
            }
        }

        file.close();
    }

    void saveBinsToFile() {
        ofstream file("bins.txt");
        for (int i = 0; i < (int)bins.size(); i++) {
            file << bins[i].toFileString() << endl;
        }
        file.close();
    }

    void loadTrucksFromFile() {
        for (int i = 0; i < (int)trucks.size(); i++) {
            delete trucks[i];
        }
        trucks.clear();

        ifstream file("trucks.txt");
        string line;

        while (getline(file, line)) {
            if (line == "") continue;

            string id, type, cap, load, route;
            stringstream ss(line);
            getline(ss, id, '|');
            getline(ss, type, '|');
            getline(ss, cap, '|');
            getline(ss, load, '|');
            getline(ss, route, '|');

            int truckId = stringToInt(id);
            double capacity = stringToDouble(cap);

            if (type == "Recycling Truck") {
                trucks.push_back(new RecyclingTruck(truckId, capacity));
            } else if (type == "Organic Compost Truck") {
                trucks.push_back(new OrganicCompostTruck(truckId, capacity));
            } else if (type == "Hazardous Waste Hauler") {
                trucks.push_back(new HazardousWasteHauler(truckId, capacity));
            }

            if (!trucks.empty()) {
                trucks[trucks.size() - 1]->setAssignedRoute(route);
            }
        }

        file.close();
    }

    void saveTrucksToFile() {
        ofstream file("trucks.txt");
        for (int i = 0; i < (int)trucks.size(); i++) {
            file << trucks[i]->toFileString() << endl;
        }
        file.close();
    }

    bool binIdExists(int id) {
        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].getBinId() == id) return true;
        }
        return false;
    }

    bool truckIdExists(int id) {
        for (int i = 0; i < (int)trucks.size(); i++) {
            if (trucks[i]->getTruckId() == id) return true;
        }
        return false;
    }

    void addSmartBin() {
        int id;
        string location, wasteType;
        double fillLevel;

        cout << "Enter Bin ID: ";
        cin >> id;

        if (binIdExists(id)) {
            cout << "Bin ID already exists." << endl;
            return;
        }

        cin.ignore(1000, '\n');
        cout << "Enter Bin Location: ";
        getline(cin, location);

        cout << "Enter Waste Type (Recyclable / Organic / Hazardous): ";
        getline(cin, wasteType);

        if (!WasteClassifier::validateWasteType(wasteType)) {
            cout << "Invalid waste type." << endl;
            return;
        }

        cout << "Enter Current Fill Level (0-100): ";
        cin >> fillLevel;

        SmartBin bin(id, location, wasteType, fillLevel);
        bins.push_back(bin);
        saveBinsToFile();

        cout << "Smart bin added successfully." << endl;
    }

    void viewSmartBins() {
        cout << "\n========== SMART BINS ==========" << endl;
        cout << left << setw(10) << "Bin ID" << setw(20) << "Location" << setw(15) << "Waste Type" << setw(15) << "Fill Level" << setw(15) << "Status" << endl;
        cout << "--------------------------------------------------------------------------" << endl;

        if (bins.empty()) {
            cout << "No smart bins available." << endl;
            return;
        }

        for (int i = 0; i < (int)bins.size(); i++) {
            bins[i].displayBinInformation();
        }
    }

    void addWasteToBin() {
        int id;
        double amount;

        cout << "Enter Bin ID: ";
        cin >> id;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].getBinId() == id) {
                cout << "Enter waste amount to add: ";
                cin >> amount;

                bins[i].addWaste(amount);
                saveBinsToFile();

                cout << "Waste added successfully." << endl;
                return;
            }
        }

        cout << "Bin not found." << endl;
    }

    void searchBin() {
        int id;
        cout << "Enter Bin ID to search: ";
        cin >> id;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].getBinId() == id) {
                cout << left << setw(10) << "Bin ID" << setw(20) << "Location" << setw(15) << "Waste Type" << setw(15) << "Fill Level" << setw(15) << "Status" << endl;
                cout << "--------------------------------------------------------------------------" << endl;
                bins[i].displayBinInformation();
                return;
            }
        }

        cout << "Bin not found." << endl;
    }

    void updateBinStatus() {
        int id;
        cout << "Enter Bin ID to update: ";
        cin >> id;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].getBinId() == id) {
                string location, wasteType;
                double fillLevel;

                cin.ignore(1000, '\n');
                cout << "Enter New Location: ";
                getline(cin, location);

                cout << "Enter New Waste Type (Recyclable / Organic / Hazardous): ";
                getline(cin, wasteType);

                if (!WasteClassifier::validateWasteType(wasteType)) {
                    cout << "Invalid waste type." << endl;
                    return;
                }

                cout << "Enter New Fill Level (0-100): ";
                cin >> fillLevel;

                bins[i].setLocation(location);
                bins[i].setWasteType(wasteType);
                bins[i].setFillLevel(fillLevel);
                saveBinsToFile();

                cout << "Bin updated successfully." << endl;
                return;
            }
        }

        cout << "Bin not found." << endl;
    }

    void addGarbageTruck() {
        int id, choice;
        double capacity;

        cout << "Enter Truck ID: ";
        cin >> id;

        if (truckIdExists(id)) {
            cout << "Truck ID already exists." << endl;
            return;
        }

        cout << "Select Truck Type:" << endl;
        cout << "1. Recycling Truck" << endl;
        cout << "2. Organic Compost Truck" << endl;
        cout << "3. Hazardous Waste Hauler" << endl;
        cout << "Enter Choice: ";
        cin >> choice;

        cout << "Enter Truck Capacity: ";
        cin >> capacity;

        if (choice == 1) {
            trucks.push_back(new RecyclingTruck(id, capacity));
        } else if (choice == 2) {
            trucks.push_back(new OrganicCompostTruck(id, capacity));
        } else if (choice == 3) {
            trucks.push_back(new HazardousWasteHauler(id, capacity));
        } else {
            cout << "Invalid truck type." << endl;
            return;
        }

        saveTrucksToFile();
        cout << "Garbage truck added successfully." << endl;
    }

    void viewGarbageTrucks() {
        cout << "\n========== GARBAGE TRUCKS ==========" << endl;
        cout << left << setw(10) << "Truck ID" << setw(25) << "Truck Type" << setw(15) << "Capacity" << setw(15) << "Load" << setw(25) << "Assigned Route" << endl;
        cout << "------------------------------------------------------------------------------------------" << endl;

        if (trucks.empty()) {
            cout << "No garbage trucks available." << endl;
            return;
        }

        for (int i = 0; i < (int)trucks.size(); i++) {
            trucks[i]->displayTruck();
        }
    }

    void classifyWasteMenu() {
        string wasteType;
        cin.ignore(1000, '\n');

        cout << "Enter Waste Type to classify: ";
        getline(cin, wasteType);

        WasteCategory* category = WasteClassifier::classifyWaste(wasteType);

        if (category == NULL) {
            cout << "Invalid waste type." << endl;
            return;
        }

        cout << "\nWaste Category: " << category->getCategoryName() << endl;
        cout << "Recycling Status: " << (category->isRecyclable() ? "Recyclable" : "Not Recyclable") << endl;
        cout << "Collection Priority: " << category->getPriority() << endl;

        WasteClassifier::generateWasteReport(wasteType);
        cout << "Waste report saved in waste_report.txt" << endl;

        delete category;
    }

    void optimizeDailyRoutes() {
        RouteOptimizer::displayDailyCollectionPlan(bins);
        RouteOptimizer::generateOptimizedRoutes(bins, trucks);
        saveBinsToFile();
        saveTrucksToFile();
    }

    void generateSystemReport() {
        int totalBins = (int)bins.size();
        int overflowBins = 0;
        int collectionNeeded = 0;
        int recyclableBins = 0, organicBins = 0, hazardousBins = 0;

        for (int i = 0; i < (int)bins.size(); i++) {
            if (bins[i].getOverflowStatus()) overflowBins++;
            if (bins[i].needsCollection()) collectionNeeded++;

            if (bins[i].getWasteType() == "Recyclable") recyclableBins++;
            else if (bins[i].getWasteType() == "Organic") organicBins++;
            else if (bins[i].getWasteType() == "Hazardous") hazardousBins++;
        }

        ofstream file("system_report.txt");
        file << "========== WASTE MANAGEMENT SYSTEM REPORT ==========" << endl;
        file << "Total Smart Bins: " << totalBins << endl;
        file << "Total Garbage Trucks: " << trucks.size() << endl;
        file << "Overflowing Bins: " << overflowBins << endl;
        file << "Bins Needing Collection: " << collectionNeeded << endl;
        file << "Recyclable Bins: " << recyclableBins << endl;
        file << "Organic Bins: " << organicBins << endl;
        file << "Hazardous Bins: " << hazardousBins << endl;
        file << "Fuel Saving Idea: Only bins with 80% or more fill level are collected." << endl;
        file.close();

        cout << "System report generated successfully in system_report.txt" << endl;
    }

    void showMenu() {
        int choice;

        do {
            cout << "\n====================================================" << endl;
            cout << " AUTOMATED WASTE MANAGEMENT & RECYCLING OPTIMIZER" << endl;
            cout << "====================================================" << endl;
            cout << "1. Add Smart Bin" << endl;
            cout << "2. View Smart Bins" << endl;
            cout << "3. Add Waste to Bin" << endl;
            cout << "4. Search Bin" << endl;
            cout << "5. Update Bin Status" << endl;
            cout << "6. Add Garbage Truck" << endl;
            cout << "7. View Garbage Trucks" << endl;
            cout << "8. Classify Waste" << endl;
            cout << "9. Optimize Daily Routes" << endl;
            cout << "10. Generate System Report" << endl;
            cout << "0. Exit" << endl;
            cout << "Enter your choice: ";
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
                case 0: cout << "Saving data and exiting program..." << endl; break;
                default: cout << "Invalid choice. Try again." << endl;
            }

        } while (choice != 0);
    }
};

// ================= Main Function =================
int main() {
    WasteManagementSystem system;
    system.showMenu();
    return 0;
}
