#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <limits>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <fstream>    // file handling: ofstream / ifstream
#include <sstream>    // in-memory log buffer
#include <streambuf>  // used to copy cout output into the log
#include <ctime>      // timestamp in the exported file

using namespace std;


// #############################################################################
// #############################################################################
//  MODULE 1  -  MEMBER 1
//  DATA FOUNDATION: Position, Enums, Order, PackageRecord,
//                   Warehouse, PathFinder (BFS), OrderManager (Hashing)
// #############################################################################
// #############################################################################

// =============================================
// 1. POSITION CLASS
// =============================================

class Position {
public:
    int row;
    int col;

    Position(int r = 0, int c = 0) {
        row = r;
        col = c;
    }

    bool operator==(const Position& p) const {
        return row == p.row && col == p.col;
    }

    bool operator!=(const Position& p) const {
        return !(*this == p);
    }
};

// =============================================
// 2. ORDER AND PACKAGE STATUS
// =============================================

enum class OrderStatus {
    Pending,
    InProgress,
    Deferred,
    Completed,
    Recovery
};

enum class PackageStatus {
    InWarehouse,
    OnCart,
    Delivered,
    Recovery
};

// =============================================
// 3. ORDER CLASS
// =============================================

class Order {
public:
    int orderID;
    Position packagePosition;
    bool isUrgent;

    Order(
        int id = 0,
        Position p = Position(),
        bool urgent = false
    ) {
        orderID = id;
        packagePosition = p;
        isUrgent = urgent;
    }
};

// =============================================
// 4. PACKAGE RECORD CLASS
// =============================================

class PackageRecord {
public:
    Position position;
    PackageStatus status;

    PackageRecord(
        Position p = Position(),
        PackageStatus s = PackageStatus::InWarehouse
    ) {
        position = p;
        status = s;
    }
};

// =============================================
// 5. WAREHOUSE
// =============================================

class Warehouse {
private:
    vector<vector<char>> grid;

public:
    Warehouse(int rows, int cols) {
        grid.resize(rows, vector<char>(cols, '.'));
    }

    int getRows() const {
        return (int)grid.size();
    }

    int getCols() const {
        return grid.empty() ? 0 : (int)grid[0].size();
    }

    bool isValid(Position p) const {
        return p.row >= 0 &&
               p.row < getRows() &&
               p.col >= 0 &&
               p.col < getCols();
    }

    char getCell(Position p) const {
        if (!isValid(p)) {
            return 'X';
        }

        return grid[p.row][p.col];
    }

    bool isBlocked(Position p) const {
        return !isValid(p) || getCell(p) == 'X';
    }

    bool placeObject(Position p, char object) {
        if (!isValid(p) || grid[p.row][p.col] != '.') {
            return false;
        }

        grid[p.row][p.col] = object;
        return true;
    }

    bool addObstacle(Position p) {
        if (!isValid(p) || grid[p.row][p.col] != '.') {
            return false;
        }

        grid[p.row][p.col] = 'X';
        return true;
    }

    void removeObstacle(Position p) {
        if (isValid(p) && grid[p.row][p.col] == 'X') {
            grid[p.row][p.col] = '.';
        }
    }

    bool removePackage(Position p) {
        if (!isValid(p) || grid[p.row][p.col] != 'P') {
            return false;
        }

        grid[p.row][p.col] = '.';
        return true;
    }

    void clearCartStart(Position p) {
        if (isValid(p) && grid[p.row][p.col] == 'C') {
            grid[p.row][p.col] = '.';
        }
    }

    void display(Position cartPosition) const {
        cout << "\n========== WAREHOUSE GRID ==========\n\n";

        for (int i = 0; i < getRows(); i++) {
            for (int j = 0; j < getCols(); j++) {

                Position p(i, j);

                if (p == cartPosition) {
                    cout << "C ";
                }
                else {
                    cout << grid[i][j] << " ";
                }
            }

            cout << endl;
        }

        cout << "\nC = Cart | P = Package | D = Delivery";
        cout << "\nS = Charging Station | X = Temporary Obstacle";
        cout << "\n. = Empty Cell\n";
    }
};

// =============================================
// 6. PATH FINDER (BFS)
// =============================================

class PathFinder {
public:

    static vector<Position> findPath(
        const Warehouse& warehouse,
        Position start,
        Position destination
    ) {

        if (!warehouse.isValid(start) ||
            !warehouse.isValid(destination) ||
            warehouse.isBlocked(start) ||
            warehouse.isBlocked(destination)) {

            return {};
        }

        queue<Position> q;

        vector<vector<bool>> visited(
            warehouse.getRows(),
            vector<bool>(warehouse.getCols(), false)
        );

        vector<vector<Position>> parent(
            warehouse.getRows(),
            vector<Position>(
                warehouse.getCols(),
                Position(-1, -1)
            )
        );

        int dr[] = {0, 1, 0, -1};
        int dc[] = {1, 0, -1, 0};

        q.push(start);

        visited[start.row][start.col] = true;

        while (!q.empty()) {

            Position current = q.front();
            q.pop();

            if (current == destination) {
                break;
            }

            for (int i = 0; i < 4; i++) {

                Position next(
                    current.row + dr[i],
                    current.col + dc[i]
                );

                if (warehouse.isValid(next) &&
                    !warehouse.isBlocked(next) &&
                    !visited[next.row][next.col]) {

                    visited[next.row][next.col] = true;

                    parent[next.row][next.col] = current;

                    q.push(next);
                }
            }
        }

        if (!visited[destination.row][destination.col]) {
            return {};
        }

        vector<Position> path;

        Position current = destination;

        while (current != start) {

            path.push_back(current);

            Position previous =
                parent[current.row][current.col];

            if (previous == Position(-1, -1)) {
                return {};
            }

            current = previous;
        }

        path.push_back(start);

        reverse(path.begin(), path.end());

        return path;
    }

    static void displayPath(const vector<Position>& path) {

        if (path.empty()) {
            cout << "No path available.\n";
            return;
        }

        for (int i = 0; i < (int)path.size(); i++) {

            cout << "("
                 << path[i].row << ","
                 << path[i].col << ")";

            if (i != (int)path.size() - 1) {
                cout << " -> ";
            }
        }

        cout << endl;
    }
};

// =============================================
// 7. ORDER PRIORITY COMPARATOR
// =============================================

class CompareOrders {
public:

    bool operator()(const Order& a, const Order& b) const {

        if (a.isUrgent != b.isUrgent) {
            return !a.isUrgent;
        }

        return a.orderID > b.orderID;
    }
};

// =============================================
// 8. ORDER MANAGER + HASHING
// =============================================

class OrderManager {
private:

    // Priority queue schedules orders
    priority_queue<
        Order,
        vector<Order>,
        CompareOrders
    > orders;

    // HASH TABLE 1:
    // Order ID -> Complete order information
    unordered_map<int, Order> orderLookup;

    // HASH TABLE 2:
    // Package ID -> Package information
    unordered_map<int, PackageRecord> packageLookup;

    // HASH TABLE 3:
    // Order ID -> Current order status
    unordered_map<int, OrderStatus> orderStatus;

public:

    // Add order and its package to the hash tables

    bool addOrder(Order order) {

        if (orderLookup.find(order.orderID) != orderLookup.end()) {

            cout << "\nERROR: Duplicate Order ID: "
                 << order.orderID << endl;

            return false;
        }

        if (packageLookup.find(order.orderID) != packageLookup.end()) {

            cout << "\nERROR: Duplicate Package ID.\n";

            return false;
        }

        orders.push(order);

        orderLookup.emplace(order.orderID, order);

        packageLookup.emplace(
            order.orderID,
            PackageRecord(
                order.packagePosition,
                PackageStatus::InWarehouse
            )
        );

        orderStatus.emplace(
            order.orderID,
            OrderStatus::Pending
        );

        cout << "\nOrder " << order.orderID << " added.";

        cout << (order.isUrgent ? " [URGENT]\n" : " [NORMAL]\n");

        return true;
    }

    // Search order using hashing

    bool searchOrder(int id, Order& foundOrder) const {

        auto it = orderLookup.find(id);

        if (it == orderLookup.end()) {
            return false;
        }

        foundOrder = it->second;

        return true;
    }

    // Search package using hashing

    bool searchPackage(
        int id,
        PackageRecord& foundPackage
    ) const {

        auto it = packageLookup.find(id);

        if (it == packageLookup.end()) {
            return false;
        }

        foundPackage = it->second;

        return true;
    }

    // Update order status

    bool updateOrderStatus(int id, OrderStatus status) {

        auto it = orderStatus.find(id);

        if (it == orderStatus.end()) {
            return false;
        }

        it->second = status;

        return true;
    }

    // Update package status

    bool updatePackageStatus(
        int id,
        PackageStatus status
    ) {

        auto it = packageLookup.find(id);

        if (it == packageLookup.end()) {
            return false;
        }

        it->second.status = status;

        return true;
    }

    // Search and display order

    void displayOrder(int id) const {

        Order foundOrder;

        if (!searchOrder(id, foundOrder)) {

            cout << "\nOrder not found!\n";

            return;
        }

        cout << "\n========== ORDER DETAILS ==========\n";

        cout << "Order ID: " << foundOrder.orderID;

        cout << "\nPackage Position: ("
             << foundOrder.packagePosition.row << ","
             << foundOrder.packagePosition.col << ")";

        cout << "\nPriority: "
             << (foundOrder.isUrgent ? "Urgent" : "Normal");

        auto it = orderStatus.find(id);

        if (it != orderStatus.end()) {

            cout << "\nStatus: ";

            switch (it->second) {
                case OrderStatus::Pending:
                    cout << "Pending";
                    break;

                case OrderStatus::InProgress:
                    cout << "In Progress";
                    break;

                case OrderStatus::Deferred:
                    cout << "Deferred";
                    break;

                case OrderStatus::Completed:
                    cout << "Completed";
                    break;

                case OrderStatus::Recovery:
                    cout << "Recovery Required";
                    break;
            }
        }

        cout << endl;
    }

    // Get next order according to priority

    bool getNextOrder(Order& order) {

        if (orders.empty()) {
            return false;
        }

        order = orders.top();

        orders.pop();

        updateOrderStatus(
            order.orderID,
            OrderStatus::InProgress
        );

        return true;
    }

    // Restore deferred orders

    void restoreDeferred(const vector<Order>& deferred) {

        for (int i = 0; i < (int)deferred.size(); i++) {

            orders.push(deferred[i]);

            updateOrderStatus(
                deferred[i].orderID,
                OrderStatus::Pending
            );
        }
    }

    bool isEmpty() const {
        return orders.empty();
    }
};

// ----------------------- END OF MODULE 1 (MEMBER 1) -------------------------


// #############################################################################
// #############################################################################
//  MODULE 2  -  MEMBER 2
//  SIMULATION ENGINE: Obstacle, ObstacleManager, ObstacleDetector,
//                     ChargingManager, Cart
// #############################################################################
// #############################################################################

// =============================================
// 9. OBSTACLE CLASS
// =============================================

class Obstacle {
public:
    Position location;
    int expiryTick;

    Obstacle(Position p, int expiry) {
        location = p;
        expiryTick = expiry;
    }
};

// =============================================
// 10. OBSTACLE MANAGER
// =============================================

class ObstacleManager {
private:

    vector<Obstacle> activeObstacles;

    int currentTick;
    int obstacleDuration;
    int generationChance;

    mt19937 rng;

public:

    ObstacleManager(int duration, int chance) {

        currentTick = 0;

        obstacleDuration = duration;

        generationChance = chance;

        random_device rd;
        rng.seed(rd());
    }

    int getCurrentTick() const {
        return currentTick;
    }

    bool hasActiveObstacles() const {
        return !activeObstacles.empty();
    }

    bool addInitialObstacle(
        Warehouse& warehouse,
        Position p
    ) {

        if (!warehouse.addObstacle(p)) {
            return false;
        }

        activeObstacles.push_back(
            Obstacle(
                p,
                currentTick + obstacleDuration
            )
        );

        return true;
    }

    // Clear expired obstacles

    void advanceTick(Warehouse& warehouse) {

        currentTick++;

        for (int i = 0; i < (int)activeObstacles.size();) {

            if (activeObstacles[i].expiryTick <= currentTick) {

                Position p = activeObstacles[i].location;

                warehouse.removeObstacle(p);

                cout << "\n*** TEMPORARY OBSTACLE CLEARED ***\n";

                cout << "Location: ("
                     << p.row << ","
                     << p.col << ")\n";

                activeObstacles.erase(
                    activeObstacles.begin() + i
                );
            }
            else {
                i++;
            }
        }
    }

    // Generate a random obstacle on the planned route

    void maybeGenerateObstacle(
        Warehouse& warehouse,
        const vector<Position>& path,
        Position cartPosition
    ) {

        if (generationChance == 0 || path.size() <= 1) {
            return;
        }

        uniform_int_distribution<int> chance(1, 100);

        if (chance(rng) > generationChance) {
            return;
        }

        vector<Position> candidates;

        for (int i = 1; i < (int)path.size(); i++) {

            Position p = path[i];

            if (p != cartPosition &&
                warehouse.getCell(p) == '.') {

                candidates.push_back(p);
            }
        }

        if (candidates.empty()) {
            return;
        }

        uniform_int_distribution<int> choose(
            0,
            (int)candidates.size() - 1
        );

        Position p = candidates[choose(rng)];

        if (warehouse.addObstacle(p)) {

            activeObstacles.push_back(
                Obstacle(
                    p,
                    currentTick + obstacleDuration
                )
            );

            cout << "\n*** NEW TEMPORARY OBSTACLE ***\n";

            cout << "Location: ("
                 << p.row << ","
                 << p.col << ")\n";

            cout << "Duration: "
                 << obstacleDuration
                 << " ticks\n";
        }
    }

    // Wait for existing obstacles to expire.
    // No new random obstacles are generated while waiting.

    void waitUntilClear(Warehouse& warehouse) {

        while (hasActiveObstacles()) {
            advanceTick(warehouse);
        }
    }
};

// =============================================
// 11. OBSTACLE DETECTOR
// =============================================

class ObstacleDetector {
public:

    bool detect(
        const Warehouse& warehouse,
        Position nextPosition
    ) {

        if (warehouse.isBlocked(nextPosition)) {

            cout << "\n!!! OBSTACLE DETECTED !!!\n";

            cout << "Blocked Position: ("
                 << nextPosition.row << ","
                 << nextPosition.col << ")\n";

            return true;
        }

        return false;
    }
};

// =============================================
// 12. CHARGING MANAGER
// =============================================

class ChargingManager {
private:

    vector<Position> stations;

    int maxBattery;
    int chargeRate;

public:

    ChargingManager(int capacity, int rate) {

        maxBattery = capacity;

        chargeRate = rate;
    }

    void addStation(Position p) {
        stations.push_back(p);
    }

    bool isStation(Position p) const {

        for (int i = 0; i < (int)stations.size(); i++) {

            if (stations[i] == p) {
                return true;
            }
        }

        return false;
    }

    int getMaxBattery() const {
        return maxBattery;
    }

    // Find nearest charger reachable with current battery

    bool findReachableCharger(
        const Warehouse& warehouse,
        Position current,
        int battery,
        Position& selected
    ) {

        int minimumDistance = numeric_limits<int>::max();

        bool found = false;

        for (int i = 0; i < (int)stations.size(); i++) {

            vector<Position> path =
                PathFinder::findPath(
                    warehouse,
                    current,
                    stations[i]
                );

            if (path.empty()) {
                continue;
            }

            int distance = (int)path.size() - 1;

            if (distance <= battery &&
                distance < minimumDistance) {

                minimumDistance = distance;

                selected = stations[i];

                found = true;
            }
        }

        return found;
    }

    // Find a charger on the planned route

    bool findChargerOnPath(
        const vector<Position>& path,
        int battery,
        int reserve,
        Position& selected
    ) {

        for (int i = 1; i < (int)path.size(); i++) {

            int distanceToStation = i;

            int remainingDistance =
                (int)path.size() - 1 - i;

            if (isStation(path[i]) &&
                distanceToStation <= battery &&
                remainingDistance + reserve <= maxBattery) {

                selected = path[i];

                return true;
            }
        }

        return false;
    }

    // Find distance to nearest reachable charger

    int nearestChargerDistance(
        const Warehouse& warehouse,
        Position current
    ) {

        int minimumDistance = numeric_limits<int>::max();

        for (int i = 0; i < (int)stations.size(); i++) {

            vector<Position> path =
                PathFinder::findPath(
                    warehouse,
                    current,
                    stations[i]
                );

            if (!path.empty()) {

                int distance = (int)path.size() - 1;

                minimumDistance = min(
                    minimumDistance,
                    distance
                );
            }
        }

        if (minimumDistance == numeric_limits<int>::max()) {
            return -1;
        }

        return minimumDistance;
    }

    // Recharge battery.
    // Return the amount of energy added.

    int chargeAt(
        Position current,
        int& battery,
        ObstacleManager& obstacleManager,
        Warehouse& warehouse
    ) {

        if (!isStation(current)) {
            return 0;
        }

        cout << "\n*** CHARGING STATION REACHED ***\n";

        int oldBattery = battery;

        while (battery < maxBattery) {

            obstacleManager.advanceTick(warehouse);

            battery = min(
                maxBattery,
                battery + chargeRate
            );

            cout << "Battery: "
                 << battery << "/"
                 << maxBattery << endl;
        }

        cout << "Charging completed!\n";

        return battery - oldBattery;
    }
};

// =============================================
// 13.CART
// =============================================

class Cart {
private:

    Position currentPosition;

    Warehouse& warehouse;

    int battery;
    int maximumBattery;

    int totalDistance;
    int totalEnergyConsumed;
    int totalEnergyCharged;

    // Move to a charger without recursive calls

    bool moveToCharger(
        Position charger,
        ObstacleManager& obstacleManager
    ) {

        while (currentPosition != charger) {

            obstacleManager.advanceTick(warehouse);

            vector<Position> path =
                PathFinder::findPath(
                    warehouse,
                    currentPosition,
                    charger
                );

            if (path.empty()) {

                if (obstacleManager.hasActiveObstacles()) {

                    cout << "\nCharger route blocked. Waiting...\n";

                    obstacleManager.waitUntilClear(warehouse);

                    continue;
                }

                cout << "\nCannot reach charging station.\n";

                return false;
            }

            int distance = (int)path.size() - 1;

            if (distance > battery) {

                cout << "\nInsufficient battery to reach charger.\n";

                return false;
            }

            Position nextPosition = path[1];

            currentPosition = nextPosition;

            battery--;

            totalDistance++;

            totalEnergyConsumed++;

            cout << "\nMoving towards charger: ("
                 << currentPosition.row << ","
                 << currentPosition.col << ")";

            cout << "\nBattery: " << battery << endl;
        }

        return true;
    }

public:

    Cart(
        Warehouse& w,
        Position start,
        int initialBattery,
        int capacity
    ) : warehouse(w) {

        currentPosition = start;

        battery = initialBattery;

        maximumBattery = capacity;

        totalDistance = 0;

        totalEnergyConsumed = 0;

        totalEnergyCharged = 0;
    }

    Position getPosition() const {
        return currentPosition;
    }

    int getBattery() const {
        return battery;
    }

    int getDistance() const {
        return totalDistance;
    }

    // Main intelligent movement function

    bool travelTo(
        Position destination,
        int reserveAfterDestination,
        ChargingManager& chargingManager,
        ObstacleManager& obstacleManager,
        ObstacleDetector& detector,
        bool waitIfBlocked = false,
        bool allowCharging = true
    ) {

        if (!warehouse.isValid(destination)) {

            cout << "\nInvalid destination.\n";

            return false;
        }

        if (reserveAfterDestination < 0 ||
            reserveAfterDestination > maximumBattery) {

            cout << "\nRequired battery reserve is not feasible.\n";

            return false;
        }

        while (currentPosition != destination) {

            // Advance simulation time

            obstacleManager.advanceTick(warehouse);

            // Calculate current route

            vector<Position> path =
                PathFinder::findPath(
                    warehouse,
                    currentPosition,
                    destination
                );

            // No route available

            if (path.empty()) {

                if (waitIfBlocked &&
                    obstacleManager.hasActiveObstacles()) {

                    cout << "\nNo route currently available.";

                    cout << "\nCart is waiting for obstacles to clear.\n";

                    obstacleManager.waitUntilClear(warehouse);

                    continue;
                }

                cout << "\nNo route available.\n";

                return false;
            }

            // Random obstacle simulation

            obstacleManager.maybeGenerateObstacle(
                warehouse,
                path,
                currentPosition
            );

            // Recalculate after obstacle generation

            path = PathFinder::findPath(
                warehouse,
                currentPosition,
                destination
            );

            if (path.empty()) {

                cout << "\nNew obstacle blocked the route.\n";

                continue;
            }

            Position nextPosition = path[1];

            // Detect obstacle

            if (detector.detect(warehouse, nextPosition)) {

                cout << "Recalculating route...\n";

                continue;
            }

            int routeDistance = (int)path.size() - 1;

            int requiredBattery =
                routeDistance + reserveAfterDestination;

            // Check whether the cart can complete the route
            // and preserve the required reserve.

            if (battery < requiredBattery) {

                if (!allowCharging) {

                    cout << "\nInsufficient battery to continue.\n";

                    return false;
                }

                Position charger;

                bool foundCharger =
                    chargingManager.findChargerOnPath(
                        path,
                        battery,
                        reserveAfterDestination,
                        charger
                    );

                if (!foundCharger) {

                    foundCharger =
                        chargingManager.findReachableCharger(
                            warehouse,
                            currentPosition,
                            battery,
                            charger
                        );
                }

                if (!foundCharger) {

                    cout << "\nNo charging station reachable "
                         << "with current battery.\n";

                    return false;
                }

                // Already at a charging station

                if (charger == currentPosition) {

                    if (battery == maximumBattery) {

                        cout << "\nFull battery but route "
                             << "is not feasible.\n";

                        return false;
                    }

                    totalEnergyCharged +=
                        chargingManager.chargeAt(
                            currentPosition,
                            battery,
                            obstacleManager,
                            warehouse
                        );

                    continue;
                }

                cout << "\nBattery reserve insufficient.";

                cout << "\nMoving to a charging station...\n";

                bool reachedCharger = moveToCharger(
                    charger,
                    obstacleManager
                );

                if (!reachedCharger) {
                    return false;
                }

                totalEnergyCharged +=
                    chargingManager.chargeAt(
                        currentPosition,
                        battery,
                        obstacleManager,
                        warehouse
                    );

                continue;
            }

            // Move one cell

            currentPosition = nextPosition;

            battery--;

            totalDistance++;

            totalEnergyConsumed++;

            cout << "\nCart Position: ("
                 << currentPosition.row << ","
                 << currentPosition.col << ")";

            cout << "\nBattery Remaining: "
                 << battery << endl;
        }

        cout << "\nDestination reached successfully!\n";

        return true;
    }

    // =========================================
    // PROCESS ALL ORDERS
    // =========================================

    int processOrders(
        OrderManager& orderManager,
        Position deliveryPoint,
        ChargingManager& chargingManager,
        ObstacleManager& obstacleManager,
        ObstacleDetector& detector,
        int totalOrders
    ) {

        vector<Order> deferred;

        int completedOrders = 0;

        int recoveryOrders = 0;

        while (true) {

            Order currentOrder;

            // Get next order from priority queue

            if (!orderManager.getNextOrder(currentOrder)) {

                if (deferred.empty()) {
                    break;
                }

                // Wait for temporary obstacles before retrying

                if (obstacleManager.hasActiveObstacles()) {

                    cout << "\nWaiting for temporary obstacles "
                         << "to clear before retrying orders.\n";

                    obstacleManager.waitUntilClear(warehouse);

                    orderManager.restoreDeferred(deferred);

                    deferred.clear();

                    continue;
                }

                cout << "\nSome orders remain unreachable.";

                cout << "\nThey will remain deferred for recovery.\n";

                break;
            }

            cout << "\n====================================\n";

            cout << "PROCESSING ORDER: "
                 << currentOrder.orderID << endl;

            cout << "Priority: "
                 << (currentOrder.isUrgent ? "URGENT" : "NORMAL")
                 << endl;

            // Check package tracking record using hashing

            PackageRecord packageRecord;

            if (!orderManager.searchPackage(
                    currentOrder.orderID,
                    packageRecord)) {

                cout << "\nPackage record not found.\n";

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Recovery
                );

                recoveryOrders++;

                continue;
            }

            if (packageRecord.status != PackageStatus::InWarehouse) {

                cout << "\nPackage is not available in warehouse.\n";

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Recovery
                );

                recoveryOrders++;

                continue;
            }

            // Find route to package

            vector<Position> packagePath =
                PathFinder::findPath(
                    warehouse,
                    currentPosition,
                    currentOrder.packagePosition
                );

            if (packagePath.empty()) {

                if (obstacleManager.hasActiveObstacles()) {

                    cout << "\nPackage route temporarily blocked.";

                    cout << "\nWaiting for obstacles to clear...\n";

                    obstacleManager.waitUntilClear(warehouse);

                    packagePath = PathFinder::findPath(
                        warehouse,
                        currentPosition,
                        currentOrder.packagePosition
                    );
                }
            }

            if (packagePath.empty()) {

                cout << "\nPackage is unreachable.";

                cout << "\nOrder deferred.\n";

                deferred.push_back(currentOrder);

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Deferred
                );

                continue;
            }

            // Calculate package-to-delivery distance

            vector<Position> deliveryPath =
                PathFinder::findPath(
                    warehouse,
                    currentOrder.packagePosition,
                    deliveryPoint
                );

            if (deliveryPath.empty() &&
                obstacleManager.hasActiveObstacles()) {

                cout << "\nDelivery route temporarily blocked.";

                cout << "\nWaiting for obstacles to clear...\n";

                obstacleManager.waitUntilClear(warehouse);

                deliveryPath = PathFinder::findPath(
                    warehouse,
                    currentOrder.packagePosition,
                    deliveryPoint
                );
            }

            if (deliveryPath.empty()) {

                cout << "\nNo delivery route available.";

                cout << "\nOrder deferred.\n";

                deferred.push_back(currentOrder);

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Deferred
                );

                continue;
            }

            // Calculate charging reserve after delivery

            int deliveryDistance =
                (int)deliveryPath.size() - 1;

            int chargerReserve =
                chargingManager.nearestChargerDistance(
                    warehouse,
                    deliveryPoint
                );

            if (chargerReserve == -1) {

                cout << "\nNo charging station is reachable "
                     << "from the delivery point.\n";

                cout << "Order deferred before package pickup.\n";

                deferred.push_back(currentOrder);

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Deferred
                );

                continue;
            }

            // Total battery reserve after reaching package:
            // delivery journey + nearest charging station journey

            int packageReserve =
                deliveryDistance + chargerReserve;

            if (packageReserve > chargingManager.getMaxBattery()) {

                cout << "\nDelivery and charging reserve exceed "
                     << "maximum battery capacity.\n";

                cout << "Order requires a different charging route.\n";

                deferred.push_back(currentOrder);

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Deferred
                );

                continue;
            }

            // Travel to package

            bool reachedPackage = travelTo(
                currentOrder.packagePosition,
                packageReserve,
                chargingManager,
                obstacleManager,
                detector,
                true
            );

            if (!reachedPackage) {

                cout << "\nCould not reach package safely.";

                cout << "\nOrder deferred.\n";

                deferred.push_back(currentOrder);

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Deferred
                );

                continue;
            }

            // Pick up package

            if (!warehouse.removePackage(
                    currentOrder.packagePosition)) {

                cout << "\nPackage could not be picked up.";

                cout << "\nOrder requires recovery.\n";

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Recovery
                );

                recoveryOrders++;

                continue;
            }

            orderManager.updatePackageStatus(
                currentOrder.orderID,
                PackageStatus::OnCart
            );

            cout << "\nPackage picked up successfully!\n";

            // Travel to delivery point

            bool delivered = travelTo(
                deliveryPoint,
                chargerReserve,
                chargingManager,
                obstacleManager,
                detector,
                true
            );

            if (delivered) {

                completedOrders++;

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Completed
                );

                orderManager.updatePackageStatus(
                    currentOrder.orderID,
                    PackageStatus::Delivered
                );

                cout << "\n*** ORDER "
                     << currentOrder.orderID
                     << " DELIVERED SUCCESSFULLY! ***\n";
            }

            else {

                // Package is still tracked as being on the cart.
                // It is not silently discarded.

                orderManager.updateOrderStatus(
                    currentOrder.orderID,
                    OrderStatus::Recovery
                );

                orderManager.updatePackageStatus(
                    currentOrder.orderID,
                    PackageStatus::Recovery
                );

                recoveryOrders++;

                cout << "\nDelivery failed.";

                cout << "\nPackage remains recorded for recovery.\n";

                break;
            }
        }

        cout << "\n====================================\n";

        cout << "ORDER PROCESSING SUMMARY\n";

        cout << "====================================\n";

        cout << "Total Orders: " << totalOrders << endl;

        cout << "Completed Orders: "
             << completedOrders << endl;

        cout << "Deferred Orders: "
             << deferred.size() << endl;

        cout << "Recovery Orders: "
             << recoveryOrders << endl;

        return completedOrders;
    }

    // =========================================
    // FINAL PERFORMANCE REPORT
    // =========================================

    void displayReport(
        int completedOrders,
        int totalOrders,
        int simulationTicks
    ) const {

        cout << "\n\n====================================\n";

        cout << "       OPTIMOVE FINAL REPORT\n";

        cout << "====================================\n";

        cout << "Total Orders: "
             << totalOrders << endl;

        cout << "Completed Orders: "
             << completedOrders << endl;

        cout << "Uncompleted Orders: "
             << totalOrders - completedOrders << endl;

        cout << "Total Distance: "
             << totalDistance << " cells\n";

        cout << "Movement Energy Consumed: "
             << totalEnergyConsumed << " units\n";

        cout << "Energy Received from Charging: "
             << totalEnergyCharged << " units\n";

        cout << "Battery Remaining: "
             << battery << " units\n";

        cout << "Simulation Time: "
             << simulationTicks << " ticks\n";

        cout << "====================================\n";
    }
};

// ----------------------- END OF MODULE 2 (MEMBER 2) -------------------------


// #############################################################################
// #############################################################################
//  MODULE 3  -  MEMBER 3
//  FILE HANDLING, INPUT VALIDATION AND MAIN:
//      TeeBuffer, LogRecorder, LogExporter,
//      readNumber, readPosition, readFileName, exportLogMenu, main()
// #############################################################################
// #############################################################################

// =============================================
// 13B. FILE HANDLING: LOG CAPTURE AND EXPORT
// (C++ equivalent of the "Export Log (.txt)" button
//  in the HTML dashboard)
// =============================================

// A stream buffer that sends every character to two places:
// the real console and an in-memory log.
class TeeBuffer : public streambuf {
private:
    streambuf* primary;
    streambuf* secondary;

protected:
    int overflow(int c) override {

        if (c == char_traits<char>::eof()) {
            return char_traits<char>::not_eof(c);
        }

        char ch = char_traits<char>::to_char_type(c);

        if (primary->sputc(ch) == char_traits<char>::eof() ||
            secondary->sputc(ch) == char_traits<char>::eof()) {

            return char_traits<char>::eof();
        }

        return c;
    }

    int sync() override {

        int a = primary->pubsync();
        int b = secondary->pubsync();

        return (a == 0 && b == 0) ? 0 : -1;
    }

public:
    TeeBuffer() : primary(nullptr), secondary(nullptr) {}

    void attach(streambuf* p, streambuf* s) {
        primary = p;
        secondary = s;
    }
};

// Records everything printed through cout while active.
// It is a global object so cout is always restored,
// even when the program ends through exit().
class LogRecorder {
private:
    ostringstream captured;
    TeeBuffer tee;
    streambuf* original;
    bool active;

public:
    LogRecorder() : original(nullptr), active(false) {}

    ~LogRecorder() {
        stop();
    }

    LogRecorder(const LogRecorder&) = delete;
    LogRecorder& operator=(const LogRecorder&) = delete;

    void start() {

        if (active) {
            return;
        }

        original = cout.rdbuf();

        tee.attach(original, captured.rdbuf());

        cout.rdbuf(&tee);

        active = true;
    }

    void stop() {

        if (!active) {
            return;
        }

        cout.flush();

        cout.rdbuf(original);

        active = false;
    }

    string getText() const {
        return captured.str();
    }
};

LogRecorder gLogRecorder;

class LogExporter {
public:

    // Reject empty names and characters that are
    // not allowed in file names.

    static bool isValidFileName(const string& name) {

        if (name.empty() || name.size() > 100) {
            return false;
        }

        const string illegal = "\\/:*?\"<>|";

        if (name.find_first_of(illegal) != string::npos) {
            return false;
        }

        return true;
    }

    static string addExtension(const string& name) {

        if (name.find('.') == string::npos) {
            return name + ".txt";
        }

        return name;
    }

    // Write the log to a file, then read it back
    // to confirm it was saved correctly.

    static bool writeLog(
        const string& fileName,
        const string& logText,
        int completedOrders,
        int totalOrders,
        int simulationTicks
    ) {

        ofstream out(fileName.c_str(), ios::out | ios::trunc);

        if (!out.is_open()) {

            cout << "\nERROR: Could not create file '"
                 << fileName << "'.\n";

            cout << "Check the folder permissions "
                 << "or choose another name.\n";

            return false;
        }

        time_t now = time(nullptr);

        char stamp[64];

        strftime(
            stamp,
            sizeof(stamp),
            "%Y-%m-%d %H:%M:%S",
            localtime(&now)
        );

        out << "====================================\n";
        out << "     OPTIMOVE SIMULATION LOG\n";
        out << "====================================\n";
        out << "Exported: " << stamp << "\n";
        out << "Orders completed: " << completedOrders
            << " of " << totalOrders << "\n";
        out << "Simulation time: " << simulationTicks
            << " ticks\n";
        out << "====================================\n";

        out << logText;

        out.flush();

        if (!out.good()) {

            cout << "\nERROR: Writing to '"
                 << fileName << "' failed "
                 << "(disk full or no permission).\n";

            out.close();

            return false;
        }

        out.close();

        // Read the file back and count its lines.

        ifstream in(fileName.c_str());

        if (!in.is_open()) {

            cout << "\nERROR: File was written but "
                 << "could not be reopened.\n";

            return false;
        }

        int lines = 0;

        string line;

        while (getline(in, line)) {
            lines++;
        }

        in.close();

        cout << "\nLog exported successfully!\n";

        cout << "File: " << fileName << "\n";

        cout << "Lines saved: " << lines << "\n";

        return true;
    }
};

// =============================================
// 14. INPUT VALIDATION
// =============================================

int readNumber(
    string message,
    int minimum,
    int maximum
) {

    int value;

    while (true) {

        cout << message;

        if (cin >> value) {

            if (value >= minimum && value <= maximum) {
                return value;
            }

            cout << "Enter a value between "
                 << minimum << " and "
                 << maximum << ".\n";
        }

        else {

            if (cin.eof()) {
                cout << "\nInput ended. Program terminated.\n";
                exit(0);
            }

            cout << "Invalid input. Numbers only.\n";

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );
        }
    }
}

// =============================================
// 15. POSITION INPUT VALIDATION
// =============================================

Position readPosition(
    string message,
    int rows,
    int cols
) {

    int r, c;

    while (true) {

        cout << message;

        if (cin >> r >> c) {

            Position p(r, c);

            if (r >= 0 && r < rows &&
                c >= 0 && c < cols) {

                return p;
            }

            cout << "Coordinates are outside the warehouse.\n";
        }

        else {

            if (cin.eof()) {
                cout << "\nInput ended. Program terminated.\n";
                exit(0);
            }

            cout << "Invalid coordinates. Enter two numbers.\n";

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );
        }
    }
}

// =============================================
// 15B. FILE NAME INPUT AND LOG EXPORT MENU
// =============================================

string readFileName(string message) {

    string name;

    while (true) {

        cout << message;

        if (cin >> name) {

            if (LogExporter::isValidFileName(name)) {
                return LogExporter::addExtension(name);
            }

            cout << "Invalid file name. Do not use spaces "
                 << "or any of  \\ / : * ? \" < > |\n";
        }

        else {

            if (cin.eof()) {
                cout << "\nInput ended. Program terminated.\n";
                exit(0);
            }

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );
        }
    }
}

void exportLogMenu(
    const string& logText,
    int completedOrders,
    int totalOrders,
    int simulationTicks
) {

    int choice = readNumber(
        "\nExport simulation log to a text file? "
        "(1 = Yes, 0 = No): ",
        0, 1
    );

    if (choice == 0) {
        cout << "\nLog export skipped.\n";
        return;
    }

    while (true) {

        string fileName = readFileName(
            "Enter file name "
            "(example: optimove_log.txt): "
        );

        if (LogExporter::writeLog(
                fileName,
                logText,
                completedOrders,
                totalOrders,
                simulationTicks)) {

            return;
        }

        int retry = readNumber(
            "\nTry a different file name? "
            "(1 = Yes, 0 = No): ",
            0, 1
        );

        if (retry == 0) {

            cout << "\nLog was not saved.\n";

            return;
        }
    }
}

// =============================================
// 16. MAIN FUNCTION
// =============================================

int main() {

    // Start recording everything shown on the console.
    // The recording is exported to a file at the end.

    gLogRecorder.start();

    cout << "\n====================================\n";

    cout << "          WELCOME TO OPTIMOVE\n";

    cout << "    SMART WAREHOUSE LOGISTICS SYSTEM\n";

    cout << "====================================\n";

    // -----------------------------------------
    // DYNAMIC WAREHOUSE DIMENSIONS
    // -----------------------------------------

    int rows;
    int cols;

    while (true) {

        rows = readNumber(
            "\nEnter warehouse rows (1-100): ",
            1, 100
        );

        cols = readNumber(
            "Enter warehouse columns (1-100): ",
            1, 100
        );

        // Need at least three cells:
        // Cart + Delivery + Charging Station

        if (rows * cols >= 3) {
            break;
        }

        cout << "\nWarehouse needs at least 3 cells "
             << "for the cart, delivery point and charger.\n";
    }

    Warehouse warehouse(rows, cols);

    // -----------------------------------------
    // CART START POSITION
    // -----------------------------------------

    Position cartStart = readPosition(
        "\nEnter cart starting position (row col): ",
        rows, cols
    );

    warehouse.placeObject(cartStart, 'C');

    // -----------------------------------------
    // DELIVERY POSITION
    // -----------------------------------------

    Position deliveryPoint;

    while (true) {

        deliveryPoint = readPosition(
            "Enter delivery position (row col): ",
            rows, cols
        );

        if (warehouse.placeObject(deliveryPoint, 'D')) {
            break;
        }

        cout << "Delivery position is already occupied.\n";
    }

    // -----------------------------------------
    // PACKAGE COUNT
    // Reserve one cell for at least one charger.
    // -----------------------------------------

    int maximumPackages = rows * cols - 3;

    int packageCount = readNumber(
        "\nEnter number of packages: ",
        0,
        maximumPackages
    );

    vector<Position> packages;

    for (int i = 0; i < packageCount; i++) {

        while (true) {

            Position p = readPosition(
                "Enter Package " + to_string(i + 1) +
                " position (row col): ",
                rows, cols
            );

            if (warehouse.placeObject(p, 'P')) {

                packages.push_back(p);

                break;
            }

            cout << "Cell occupied. Choose another position.\n";
        }
    }

    // -----------------------------------------
    // BATTERY CONFIGURATION
    // -----------------------------------------

    int maxBattery = readNumber(
        "\nEnter maximum battery capacity: ",
        1, 100000
    );

    int chargeRate = readNumber(
        "Enter charging rate per tick: ",
        1, 100000
    );

    int initialBattery = readNumber(
        "Enter initial cart battery: ",
        1, maxBattery
    );

    ChargingManager chargingManager(
        maxBattery,
        chargeRate
    );

    // -----------------------------------------
    // CHARGING STATIONS
    // -----------------------------------------

    int remainingCells =
        rows * cols - 2 - packageCount;

    int stationCount = readNumber(
        "\nEnter number of charging stations: ",
        1,
        remainingCells
    );

    for (int i = 0; i < stationCount; i++) {

        while (true) {

            Position p = readPosition(
                "Enter Charging Station " +
                to_string(i + 1) +
                " position (row col): ",
                rows, cols
            );

            if (warehouse.placeObject(p, 'S')) {

                chargingManager.addStation(p);

                break;
            }

            cout << "Cell occupied. Choose another position.\n";
        }
    }

    // -----------------------------------------
    // TEMPORARY OBSTACLE CONFIGURATION
    // -----------------------------------------

    int obstacleDuration = readNumber(
        "\nEnter obstacle duration in ticks: ",
        1, 10000
    );

    int obstacleChance = readNumber(
        "Enter random obstacle probability (0-100): ",
        0, 100
    );

    ObstacleManager obstacleManager(
        obstacleDuration,
        obstacleChance
    );

    int availableForObstacles =
        rows * cols -
        2 -
        packageCount -
        stationCount;

    int initialObstacleCount = readNumber(
        "\nEnter number of initial obstacles: ",
        0,
        availableForObstacles
    );

    for (int i = 0; i < initialObstacleCount; i++) {

        while (true) {

            Position p = readPosition(
                "Enter Initial Obstacle " +
                to_string(i + 1) +
                " position (row col): ",
                rows, cols
            );

            if (obstacleManager.addInitialObstacle(
                    warehouse, p)) {

                break;
            }

            cout << "Cell occupied. Choose another position.\n";
        }
    }

    // -----------------------------------------
    // ORDER MANAGER + HASH TABLES
    // -----------------------------------------

    OrderManager orderManager;

    for (int i = 0; i < packageCount; i++) {

        int orderID = 101 + i;

        int urgency = readNumber(
            "\nOrder " + to_string(orderID) +
            " urgency (1 = Urgent, 0 = Normal): ",
            0, 1
        );

        Order order(
            orderID,
            packages[i],
            urgency == 1
        );

        orderManager.addOrder(order);
    }

    // -----------------------------------------
    // HASHING TEST
    // -----------------------------------------

    int searchID;

    if (packageCount > 0) {

        searchID = readNumber(
            "\nEnter an Order ID to test hashing "
            "(101 to " +
            to_string(100 + packageCount) + "): ",
            101,
            100 + packageCount
        );

        cout << "\nSearching through hash table...\n";

        orderManager.displayOrder(searchID);

        PackageRecord foundPackage;

        if (orderManager.searchPackage(
                searchID,
                foundPackage)) {

            cout << "\nPackage Hash Lookup Successful!";

            cout << "\nPackage Position: ("
                 << foundPackage.position.row << ","
                 << foundPackage.position.col << ")\n";
        }
    }

    // -----------------------------------------
    // CREATE CART
    // -----------------------------------------

    Cart cart(
        warehouse,
        cartStart,
        initialBattery,
        maxBattery
    );

    ObstacleDetector detector;

    // Remove the temporary cart marker.
    // The cart's actual position is tracked by Cart.

    warehouse.clearCartStart(cartStart);

    // -----------------------------------------
    // DISPLAY INITIAL WAREHOUSE
    // -----------------------------------------

    cout << "\nINITIAL WAREHOUSE:\n";

    warehouse.display(cart.getPosition());

    // -----------------------------------------
    // START SIMULATION
    // -----------------------------------------

    cout << "\n====================================\n";

    cout << "        SIMULATION STARTED\n";

    cout << "====================================\n";

    int completedOrders = cart.processOrders(
        orderManager,
        deliveryPoint,
        chargingManager,
        obstacleManager,
        detector,
        packageCount
    );

    // -----------------------------------------
    // FINAL WAREHOUSE
    // -----------------------------------------

    cout << "\nFINAL WAREHOUSE:\n";

    warehouse.display(cart.getPosition());

    // -----------------------------------------
    // FINAL REPORT
    // -----------------------------------------

    cart.displayReport(
        completedOrders,
        packageCount,
        obstacleManager.getCurrentTick()
    );

    cout << "\nOPTIMOVE SIMULATION FINISHED.\n";

    // -----------------------------------------
    // FILE HANDLING: EXPORT LOG
    // Stop recording first so the export menu
    // is not written into the log itself.
    // -----------------------------------------

    gLogRecorder.stop();

    exportLogMenu(
        gLogRecorder.getText(),
        completedOrders,
        packageCount,
        obstacleManager.getCurrentTick()
    );

    return 0;
}

// ----------------------- END OF MODULE 3 (MEMBER 3) -------------------------