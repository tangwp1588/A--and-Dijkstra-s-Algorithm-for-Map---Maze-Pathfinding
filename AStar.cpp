#include <bits/stdc++.h>
#include <chrono>
#include <iomanip>

using namespace std;
// To play around with different map, please scroll to the bottom to change the filename in main function

// Dynamic grid structure
vector<vector<int>> grid;
int ROW, COL;

typedef pair<int, int> Pair;
typedef pair<double, pair<int, int>> pPair;

struct cell {
    int parent_i, parent_j;
    double f, g, h;
};

// Timing statistics structure
struct TimingStats {
    double best_time;
    double worst_time;
    double total_time;
    double current_time;  // Added current execution time
    int count;
    
    TimingStats() : best_time(std::numeric_limits<double>::max()),
                    worst_time(0.0),
                    total_time(0.0),
                    current_time(0.0),  // Initialize current time
                    count(0) {}
    
    void addMeasurement(double time) {
        current_time = time;  // Store the current execution time
        if (time < best_time) {
            best_time = time;
        }
        if (time > worst_time) {
            worst_time = time;
        }
        total_time += time;
        count++;
    }
    
    double getAverage() const {
        return count > 0 ? total_time / count : 0.0;
    }
    
    void display() const {
        if (count > 0) {
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "\nTiming Statistics:" << std::endl;
            std::cout << "Current execution time: " << current_time << " ms" << std::endl;  // Display current time
            std::cout << "Best time: " << best_time << " ms" << std::endl;
            std::cout << "Average time: " << getAverage() << " ms" << std::endl;
            std::cout << "Worst time: " << worst_time << " ms" << std::endl;
            std::cout << "Total executions: " << count << std::endl;
        } else {
            std::cout << "\nNo timing measurements recorded." << std::endl;
        }
    }
};

TimingStats pathfindingStats;

// Function to read map from file
pair<Pair, Pair> readMapFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Unable to open file: " + filename);
    }

    vector<string> lines;
    string line;
    Pair src, dest;
    
    // Read all lines and find dimensions
    while (getline(file, line)) {
        // Remove spaces from the line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    if (lines.empty()) {
        throw runtime_error("Empty file");
    }
    
    ROW = lines.size();
    COL = lines[0].length();
    
    // Initialize grid with proper dimensions
    grid = vector<vector<int>>(ROW, vector<int>(COL));
    
    // Parse the map
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            char c = lines[i][j];
            if (c == 'X' || c == 'x') {
                grid[i][j] = 1;  // Obstacle
            } else if (c == 'S' || c == 's') {
                src = make_pair(i, j);
                grid[i][j] = 0;
            } else if (c == 'E' || c == 'e') {
                dest = make_pair(i, j);
                grid[i][j] = 0;
            } else {
                grid[i][j] = 0;  // Free path
            }
        }
    }
    
    return make_pair(src, dest);
}

bool isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

bool isUnBlocked(int row, int col)
{
    return (grid[row][col] == 0);
}

bool isDestination(int row, int col, Pair dest)
{
    return (row == dest.first && col == dest.second);
}

double calculateHValue(int row, int col, Pair dest)
{
    return ((double)sqrt((row - dest.first) * (row - dest.first)
        + (col - dest.second) * (col - dest.second)));
}


vector<Pair> tracePath(vector<vector<cell>>& cellDetails, Pair dest)
{
    vector<Pair> path;
    int row = dest.first;
    int col = dest.second;

    while (!(cellDetails[row][col].parent_i == row
             && cellDetails[row][col].parent_j == col)) {
        path.push_back(make_pair(row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
    path.push_back(make_pair(row, col));
    reverse(path.begin(), path.end());
    return path;
}

// Modified A* search that returns the path instead of printing it
vector<Pair> aStarSearch(Pair src, Pair dest) {
    // Start timing at the very beginning
    auto start_time = chrono::high_resolution_clock::now();
    vector<Pair> path;
    
    // Validation checks
    if (!isValid(src.first, src.second)) {
        cout << "Source is invalid" << endl;
        return path;
    }

    if (!isValid(dest.first, dest.second)) {
        cout << "Destination is invalid" << endl;
        return path;
    }

    if (!isUnBlocked(src.first, src.second) || !isUnBlocked(dest.first, dest.second)) {
        cout << "Source or destination is blocked" << endl;
        return path;
    }

    if (isDestination(src.first, src.second, dest)) {
        path.push_back(src);
        // Calculate time even for immediate return
        auto end_time = chrono::high_resolution_clock::now();
        double execution_time = chrono::duration<double, milli>(end_time - start_time).count();
        pathfindingStats.addMeasurement(execution_time);
        return path;
    }

    // Initialize closed list and cell details as before
    vector<vector<bool>> closedList(ROW, vector<bool>(COL, false));
    vector<vector<cell>> cellDetails(ROW, vector<cell>(COL));

    // Initialize cell details
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialize starting point
    int i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    set<pPair> openList;
    openList.insert(make_pair(0.0, make_pair(i, j)));

    // Direction arrays for 8-directional movement
    int dRow[] = {-1, 0, 1, 0, -1, -1, 1, 1};
    int dCol[] = {0, 1, 0, -1, -1, 1, -1, 1};

    while (!openList.empty()) {
        pPair p = *openList.begin();
        openList.erase(openList.begin());

        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        for (int dir = 0; dir < 8; dir++) {
            int newRow = i + dRow[dir];
            int newCol = j + dCol[dir];

            if (isValid(newRow, newCol)) {
                if (isDestination(newRow, newCol, dest)) {
                    cellDetails[newRow][newCol].parent_i = i;
                    cellDetails[newRow][newCol].parent_j = j;
                    path = tracePath(cellDetails, dest);
                    // Calculate time before returning found path
                    auto end_time = chrono::high_resolution_clock::now();
                    double execution_time = chrono::duration<double, milli>(end_time - start_time).count();
                    pathfindingStats.addMeasurement(execution_time);
                    return path;
                }
                else if (!closedList[newRow][newCol] && isUnBlocked(newRow, newCol)) {
                    double gNew = cellDetails[i][j].g + ((dir < 4) ? 1.0 : 1.414);
                    double hNew = calculateHValue(newRow, newCol, dest);
                    double fNew = gNew + hNew;

                    if (cellDetails[newRow][newCol].f == FLT_MAX ||
                        cellDetails[newRow][newCol].f > fNew) {
                        openList.insert(make_pair(fNew, make_pair(newRow, newCol)));
                        cellDetails[newRow][newCol].f = fNew;
                        cellDetails[newRow][newCol].g = gNew;
                        cellDetails[newRow][newCol].h = hNew;
                        cellDetails[newRow][newCol].parent_i = i;
                        cellDetails[newRow][newCol].parent_j = j;
                    }
                }
            }
        }
    }

    // Calculate time even when no path is found
    auto end_time = chrono::high_resolution_clock::now();
    double execution_time = chrono::duration<double, milli>(end_time - start_time).count();
    pathfindingStats.addMeasurement(execution_time);
    
    cout << "Failed to find path to destination" << endl;
    return path;
}

// Modified function to add an obstacle and recalculate path
vector<Pair> handleNewObstacle(int row, int col, Pair current, Pair dest) {
    if (!isValid(row, col)) {
        cout << "Invalid obstacle position" << endl;
        return vector<Pair>();
    }
    
    // Check if the new obstacle blocks the current position or destination
    if ((row == current.first && col == current.second) ||
        (row == dest.first && col == dest.second)) {
        cout << "Cannot place obstacle on current position or destination" << endl;
        return vector<Pair>();
    }
    
    // Add the new obstacle
    grid[row][col] = 1;
    cout << "Added obstacle at (" << row << "," << col << ")" << endl;
    
    // Try to find a new path
    vector<Pair> newPath = aStarSearch(current, dest);
    
    // If no path is found, try to find alternative routes by exploring nearby valid positions
    if (newPath.empty()) {
        cout << "Trying to find alternative routes..." << endl;
        
        // Check nearby positions for a valid starting point
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                
                int newRow = current.first + dx;
                int newCol = current.second + dy;
                
                if (isValid(newRow, newCol) && isUnBlocked(newRow, newCol)) {
                    Pair altStart = make_pair(newRow, newCol);
                    newPath = aStarSearch(altStart, dest);
                    if (!newPath.empty()) {
                        // Insert the current position at the start of the path
                        newPath.insert(newPath.begin(), current);
                        return newPath;
                    }
                }
            }
        }
    }
    
    return newPath;
}

// Function to visualize the grid and path
// Updated visualization function with row and column labels
void visualizeGrid(const vector<Pair>& path, Pair current, Pair dest) {
    vector<vector<char>> display(ROW, vector<char>(COL, '.'));
    
    // Mark obstacles
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            if(grid[i][j] == 1) {
                display[i][j] = 'X';
            }
        }
    }
    
    // Mark path
    for(const auto& p : path) {
        display[p.first][p.second] = 'o';
    }
    
    // Mark current position and destination
    display[current.first][current.second] = 'S';
    display[dest.first][dest.second] = 'E';
    
    // Print the grid with labels
    cout << "\nGrid visualization:" << endl;
    cout << ". = Empty  X = Obstacle  o = Path  S = Start Point  E = End Point\n" << endl;
    
    // Print column numbers
    cout << "     ";
    for(int j = 0; j < COL; j++) {
        printf("%d", j/10);
        cout << " ";
    }
    cout << "\n     ";
    for(int j = 0; j < COL; j++) {
        printf("%d", j%10);
        cout << " ";
    }
    cout << endl;
    
    // Print grid with row numbers
    for(int i = 0; i < ROW; i++) {
        if (i < 10) {
            cout << " ";  // Space for single-digit row numbers
        }
        cout << i << " [ ";  // Row number with border
        for (int j = 0; j < COL; j++) {
            // Add color to specific characters
            if (display[i][j] == 'X') {
                cout << "\033[33mX\033[0m ";  // Yellow for X
            } 
            else if (display[i][j] == 'o') {
                cout << "\033[31mo\033[0m ";  // Red for o
            } 
            else if (display[i][j] == '.') {
                cout << "\033[90m.\033[0m ";  // Grey for .
            }
            else if (display[i][j] == 'S') {
                cout << "\033[36mS\033[0m ";  // Cyan for S
            } 
            else if (display[i][j] == 'E') {
                cout << "\033[32mE\033[0m ";  // Green for E
            } 
            else {
                cout << display[i][j] << " ";
            }
        }
        cout << "]" << endl;  // Right border
    }
}

Pair getUserObstacle() {
    int row, col;
    while (true) {
        cout << "\nEnter obstacle position (row col), or -1 -1 to finish: ";
        cin >> row >> col;
        
        if (row == -1 && col == -1) {
            return make_pair(-1, -1);
        }
        
        if (row >= 0 && row < ROW && col >= 0 && col < COL) {
            return make_pair(row, col);
        }
        cout << "Invalid coordinates! Row should be 0-" << ROW-1 
             << " and Col should be 0-" << COL-1 << endl;
    }
}

// New function for step-by-step path execution with interactive obstacle addition
void executePathWithInteraction(vector<Pair> initialPath, Pair src, Pair dest) {
    if (initialPath.empty()) {
        cout << "No initial path found!" << endl;
        return;
    }
    
    vector<Pair> currentPath = initialPath;
    Pair currentPos = src;
    size_t pathIndex = 0;
    
    // cout << "\nPath execution started!" << endl;
    // cout << "At each step, you can:" << endl;
    // cout << "1. Enter coordinates to add an obstacle" << endl;
    // cout << "2. Enter -1 -1 to end simulation" << endl;
    
    while (pathIndex < currentPath.size()) {
        visualizeGrid(currentPath, currentPos, dest);
        
        Pair nextPos = currentPath[pathIndex];
        
        cout << "\nCurrent position: (" << currentPos.first << "," 
             << currentPos.second << ")" << endl;
        cout << "Next position: (" << nextPos.first << "," 
             << nextPos.second << ")" << endl;
        
        // Display current timing stats after each step
        pathfindingStats.display();
        
        bool validObstacleAdded = false;
        while (!validObstacleAdded) {
            Pair obstacle = getUserObstacle();
            
            if (obstacle.first == -1 && obstacle.second == -1) {
                cout << "Simulation ended by user" << endl;
                pathfindingStats.display();  // Display final stats
                return;
            }
            
            if (obstacle.first != -1) {
                if (obstacle == nextPos || obstacle == dest) {
                    cout << "Cannot place obstacle on next position or destination!" << endl;
                    continue;
                }
                
                if (grid[obstacle.first][obstacle.second] == 1) {
                    cout << "Obstacle already exists at this position!" << endl;
                    continue;
                }
                
                vector<Pair> newPath = handleNewObstacle(obstacle.first, 
                                                        obstacle.second, 
                                                        currentPos, 
                                                        dest);
                
                if (newPath.empty()) {
                    cout << "No valid path found after adding obstacle! "
                         << "Please try different obstacle position." << endl;
                    grid[obstacle.first][obstacle.second] = 0;
                    continue;
                }
                
                currentPath = newPath;
                pathIndex = 0;
                cout << "New path calculated!" << endl;
                validObstacleAdded = true;
                
                // Display stats after recalculating path
                pathfindingStats.display();
            }
        }
        
        if (!validObstacleAdded) {
            currentPos = nextPos;
            pathIndex++;
            
            if (currentPos == dest) {
                cout << "\nDestination reached!" << endl;
                visualizeGrid(currentPath, currentPos, dest);
                pathfindingStats.display();  // Display final stats
                break;
            }
        }
    }
}

//parcel part
struct Package {
    string id;
    int urgency;
    int weight;
    string description;
};

bool comparePackages(const Package& a, const Package& b) {
    if (a.urgency != b.urgency) {
        return a.urgency > b.urgency;  // Higher urgency first
    }
    return a.weight > b.weight;  // Higher weight first if same urgency level
}

void displayShippingOrder(const vector<Package>& packages) {
    cout << "\nShipping Order:" << endl;
    cout << "No.\tID\tUrgency\tWeight\tDescription" << endl;
    cout << "----------------------------------------------------------" << endl;
    int count = 1;
    for (const auto& p : packages) {
        cout << count << "\t" << p.id << "\t" << p.urgency << "\t" 
             << p.weight << "\t" << p.description << endl;
        count++;
    }
    cout << endl;
}

int main() {
    vector<Package> packages = {
        // ID, Urgency, Weight, Description
        {"PKG2", 1, 10, "Gold Bars"},
        {"PKG7", 1, 3, "A Secret Letter"},
        {"PKG6", 2, 15, "An Elephant"},
        {"PKG3", 2, 7, "A Bomb"},
        {"PKG1", 2, 5, "A Tiny Elephant"},
        {"PKG10", 2, 4, "A Box of Chocolates"},
        {"PKG9", 3, 13, "The President"},
        {"PKG4", 4, 4, "The President"},
        {"PKG8", 4, 4, "Child Labourers"},
        {"PKG5", 5, 3, "Chinese Propaganda Books"}};

    // Sort packages by urgency and weight
    sort(packages.begin(), packages.end(), comparePackages);

    // Display the shipping order
    displayShippingOrder(packages);

    try {
        // Read map from file
        
        // string filename = "map100x100.txt";
        string filename = "maze101x101.txt"; 
        auto [src, dest] = readMapFromFile(filename);
        
        cout << "Map loaded successfully!" << endl;
        cout << "Grid dimensions: " << ROW << "x" << COL << endl;
        cout << "Start position: (" << src.first << "," << src.second << ")" << endl;
        cout << "End position: (" << dest.first << "," << dest.second << ")" << endl;
        
        // Calculate initial path
        vector<Pair> initialPath = aStarSearch(src, dest);
        
        if (!initialPath.empty()) {
            cout << "Initial path found! Starting simulation..." << endl;
            executePathWithInteraction(initialPath, src, dest);
        } else {
            cout << "No initial path found!" << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}