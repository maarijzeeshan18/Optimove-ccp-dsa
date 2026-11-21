# OptiMove — Smart Warehouse Logistics System

**OptiMove** is a C++ simulation and logistics management system for smart warehouses, developed as a **Data Structures and Algorithms (DSA)** Complex Computing Problem (CCP) project[cite: 4]. It models an $R \times C$ warehouse grid where an autonomous cart fulfills prioritized orders, navigates around dynamic obstacles, manages battery reserves and charging stations, and exports execution logs[cite: 6, 10].

---

## Team Members

| Roll No. | Name | Module |
| :--- | :--- | :--- |
| **CT-25202** | Syeda Zujaja Tun Noor | Module 3: File Handling, Input Validation & Main[cite: 27] |
| **CT-25203** | Syeda Aymen Tariq | Module 1: Data Foundation[cite: 27] |
| **CT-25232** | Maarij Zeeshan | Module 2: Simulation Engine[cite: 27] |

---

## System Architecture & Modules

The project follows a one-way dependency structure (**Module 1 → Module 2 → Module 3**)[cite: 10]:

### Module 1: Data Foundation
* **`Position`**: Stores `(row, col)` coordinates with equality (`==`) and inequality (`!=`) operators[cite: 11].
* **`OrderStatus` & `PackageStatus`**: Enumerations tracking order (`Pending`, `InProgress`, `Deferred`, `Completed`, `Recovery`) and package (`InWarehouse`, `OnCart`, `Delivered`, `Recovery`) states[cite: 11].
* **`Order` & `PackageRecord`**: Represent order requests (`orderID`, `packagePosition`, `isUrgent`) and package tracking records (`position`, `status`)[cite: 12].
* **`Warehouse`**: Manages the 2D grid (`vector<vector<char>>`) with cells for empty space (`.`), cart (`C`), packages (`P`), delivery point (`D`), charging stations (`S`), and temporary obstacles (`X`)[cite: 7, 12, 13].
* **`PathFinder`**: Implements **Breadth-First Search (BFS)** with 4-directional movement (`up`, `down`, `left`, `right`) and parent backtracking to compute the shortest path on an unweighted grid[cite: 8, 13, 14].
* **`CompareOrders`**: Custom comparator prioritizing `isUrgent` orders first, with smaller `orderID` breaking ties[cite: 8, 14].
* **`OrderManager`**: Combines a `priority_queue` for scheduling with three `unordered_map` hash tables (`orderLookup`, `packageLookup`, `orderStatus`) for fast lookups and state updates[cite: 8, 14, 15].

### Module 2: Simulation Engine
* **`Obstacle`**: Stores a temporary obstacle's `location` and `expiryTick`[cite: 16].
* **`ObstacleManager`**: Advances simulation ticks (`advanceTick`), spawns random obstacles along planned routes using `mt19937` (`maybeGenerateObstacle`), and waits for active obstacles to clear (`waitUntilClear`)[cite: 16, 17].
* **`ObstacleDetector`**: Checks whether the next grid cell is blocked (`isBlocked`) and triggers route recalculation[cite: 17].
* **`ChargingManager`**: Tracks charging stations (`S`), battery capacity, and charge rate, finding chargers on-path or nearest reachable chargers and recharging carts across simulation ticks[cite: 17, 18].
* **`Cart`**: Handles intelligent movement (`travelTo`) with battery reserve checks and charging rerouting, processes orders (`processOrders`) with deferral/recovery handling, and prints the final summary (`displayReport`)[cite: 18, 19, 20, 21, 22].

### Module 3: File Handling, Input Validation & Main
* **`TeeBuffer` & `LogRecorder`**: Intercept `std::cout` output so simulation logs are displayed on the console and captured in memory simultaneously[cite: 22].
* **`LogExporter` & `exportLogMenu`**: Validate file names, append `.txt` extensions if needed, write timestamped logs with summary metrics, and verify exported files[cite: 23, 24].
* **Input Validation (`readNumber`, `readPosition`, `readFileName`) & `main()`**: Enforce valid numeric ranges, grid bounds, and filenames while wiring the full setup and simulation loop[cite: 23, 24, 25, 26].

---

## Core DSA Concepts Used

* **2D Grid (`vector<vector<char>>`)**: $O(1)$ cell access for warehouse layout mapping[cite: 7, 8].
* **BFS with Queue**: Finds shortest paths in $O(R \times C)$ time on an unweighted grid[cite: 7, 8, 9].
* **Priority Queue (Heap)**: Selects the highest-priority pending order in $O(\log N)$ time[cite: 7, 8, 9].
* **Hash Maps (`unordered_map`)**: Provide $O(1)$ average lookup and status updates by ID[cite: 7, 8, 9].
* **Discrete-Time Tick Simulation**: Tracks obstacle lifetimes, waiting, and charging over simulation ticks[cite: 7, 8].

---

## Compilation & Execution

Compile with a standard C++17 (or later) compiler:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o optimove
