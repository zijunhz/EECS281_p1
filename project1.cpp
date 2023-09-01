#include <getopt.h>
#include <cctype>
#include <cstring>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Error {
   public:
    string errMessage;
    Error() {}
    Error(const string s)
        : errMessage(s) {}
};

class MazeSolving {
   public:
    MazeSolving()
        : nColor(0), height(0), width(0), triedSolve(false), outputType(MAP), hasStart(false), hasTarget(false), targetReached(false) {}
    /**
     * @brief Get options from command line arguments and set everything done. Error in options are handled in this step.
     *
     * @param argc
     * @param argv
     */
    void getOptions(int argc, char** argv);
    /**
     * @brief read maze from input. read # of colors, height, width, and the whole maze. Check any error.
     *
     */
    void readMaze();
    /**
     * @brief solve the maze by the given search method.
     * solution will be stored in vis and move in solve and be passed by print ans directly
     *
     */
    void solve();

   private:
    const int32_t dx[4] = {-1, 0, 1, 0};
    const int32_t dy[4] = {0, 1, 0, -1};
    enum SearchMethod {
        BFS,
        DFS
    };
    enum Move {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };
    enum OutputType {
        MAP,
        LIST
    };
    /**
     * @brief a node type used in deque
     * x y curColor curSteps move fromColor
     *
     */
    class Node {
       public:
        Node() {}
        Node(int32_t x, int32_t y, ushort c, uint32_t steps, Move move, ushort fromColor)
            : x(x), y(y), c(c), steps(steps), move(move), fromColor(fromColor) {}
        void updateNode(int32_t x, int32_t y, ushort c, uint32_t steps, Move move, ushort fromColor) {
            this->x = x;
            this->y = y;
            this->c = c;
            this->steps = steps;
            this->move = move;
            this->fromColor = fromColor;
        }
        int32_t x;
        int32_t y;
        ushort c;  // abcd...z for corresponding activated button; ^ for trap  // 0...26
        uint32_t steps;
        Move move;
        ushort fromColor;
        class NodeSort {
           public:
            bool operator()(const Node& n1, const Node& n2) {
                return n1.steps < n2.steps;
            }
        };
        Node& operator=(const Node& n) {
            copyFrom(n);
            return *this;
        }
        Node(const Node& n) {
            copyFrom(n);
        }

       private:
        void copyFrom(const Node& n) {
            x = n.x;
            y = n.y;
            c = n.c;
            steps = n.steps;
            move = n.move;
            fromColor = n.fromColor;
        }
    };

    uint32_t nColor;
    uint32_t height;
    uint32_t width;
    vector<vector<char>> mazeMap;
    bool triedSolve;
    OutputType outputType;
    SearchMethod searchMethod;
    Node start;
    bool hasStart;
    Node target;
    bool hasTarget;

    bool targetReached;

    /**
     * @brief whether cur can be investigate later. cur cannot be investigate if is has already been investigated or cur is out of the map
     *
     * @param cur
     * @param vis
     * @return true
     * @return false
     */
    inline bool isValid(const Node& cur, const vector<vector<vector<int32_t>>>& vis);
    inline bool isInMap(const Node& cur);
    inline ushort c2n(char c) {
        return c == '^' ? 0 : ushort(islower(c) ? c - 'a' + 1 : c - 'A' + 1);
    }
    inline char n2c(uint32_t n) {
        return n == 0 ? '^' : char('a' + n - 1);
    }
    void outputSol(const vector<vector<vector<int32_t>>>& vis, const vector<vector<vector<Move>>>& move, const vector<vector<vector<ushort>>>& fromColor);
    void outputNoSol(const vector<vector<vector<int32_t>>>& vis);
};

int main(int argc, char** argv) {
    try {
        ios_base::sync_with_stdio(false);
        MazeSolving maze;
        maze.getOptions(argc, argv);
        maze.readMaze();
        maze.solve();
    } catch (Error& err) {
        cerr << err.errMessage << endl;
        return 1;
    } catch (std::runtime_error& e) {
        cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void MazeSolving::getOptions(int argc, char** argv) {
    int optionIdx = 0;
    int option = 0;
    opterr = 0;
    struct option longOpts[] = {{"queue", no_argument, nullptr, 'q'},
                                {"stack", no_argument, nullptr, 's'},
                                {"output", required_argument, nullptr, 'o'},
                                {"help", no_argument, nullptr, 'h'}};

    bool outputTypeSpecified = false;
    bool searchMethodSpecified = false;

    while ((option = getopt_long(argc, argv, "qso:h", longOpts, &optionIdx)) != -1) {
        switch (option) {
            case 'h':
                cout << "I'm so lazy writing a help that is already available at https://eecs281staff.github.io/p1-puzzle/#puzzle-options\n";
                exit(0);
            case 'q':
                if (searchMethodSpecified) {
                    throw(Error("Error: Can not have both stack and queue"));
                }
                searchMethodSpecified = true;
                searchMethod = BFS;
                break;
            case 's':
                if (searchMethodSpecified) {
                    throw(Error("Error: Can not have both stack and queue"));
                }
                searchMethodSpecified = true;
                searchMethod = DFS;
                break;
            case 'o':
                if (outputTypeSpecified) {
                    throw(Error("Error: Redundant output type"));
                }
                outputTypeSpecified = true;
                if (strcmp(optarg, "list") == 0) {
                    outputType = LIST;
                } else if (strcmp(optarg, "map") == 0) {
                    outputType = MAP;
                } else {
                    throw(Error("Error: Invalid output mode"));
                }
                break;
            default:
                throw(Error("Error: Unknown option"));
        }
    }
    if (!searchMethodSpecified) {
        throw(Error("Error: Must have at least one of stack or queue"));
    }
}

void MazeSolving::readMaze() {
    cin >> nColor >> height >> width;
    if (nColor > 26) {
        throw(Error("Error: The number of colors must be in [0, 26]"));
    }
    if (height <= 0 || width <= 0) {
        throw(Error("Error: The size of the maze cannot be 0"));
    }
    mazeMap.resize(height);
    for (uint32_t i = 0; i < height; i++) {
        mazeMap[i].resize(width);
        for (uint32_t j = 0; j < width; j++) {
            if (!(cin >> mazeMap[i][j])) {
                throw(Error("Error: The map size does not agree with height*width"));
            }
            switch (mazeMap[i][j]) {
                case '@':
                    if (hasStart) {
                        throw(Error("Error: the maze cannot have more than 1 start"));
                    }
                    hasStart = true;
                    start.updateNode(i, j, 0, 0, UP, 0);
                    break;
                case '?':
                    if (hasTarget) {
                        throw(Error("Error: the maze cannot have more than 1 target"));
                    }
                    hasTarget = true;
                    target.updateNode(i, j, 0, 0, UP, 0);
                    break;
                case '.':
                    break;
                case '#':
                    break;
                case '^':
                    break;
                default:
                    if (islower(mazeMap[i][j])) {
                        if (uint32_t(mazeMap[i][j] - 'a' + 1) > nColor) {
                            throw(Error("Error: Button color exceeds # of colors"));
                        }
                    } else if (isupper(mazeMap[i][j])) {
                        if (uint32_t(mazeMap[i][j] - 'A' + 1) > nColor) {
                            throw(Error("Error: Door color exceeds # of colors"));
                        }
                    } else {
                        throw(Error("Error: Invalid map character"));
                    }
            }
        }
    }
    char c;
    if (cin >> c) {
        throw(Error("Error: The map size does not agree with height*width"));
    }
    if ((!hasStart) || (!hasTarget)) {
        throw(Error("Error: Must has 1 and only 1 start and target"));
    }
}

void MazeSolving::solve() {
    vector<vector<vector<int32_t>>> vis(nColor + 1, vector<vector<int32_t>>(height, vector<int32_t>(width, -1)));
    vector<vector<vector<Move>>> move(nColor + 1, vector<vector<Move>>(height, vector<Move>(width, UP)));
    vector<vector<vector<ushort>>> fromColor(nColor + 1, vector<vector<ushort>>(height, vector<ushort>(width, -1)));
    deque<Node> deq;
    deq.push_back(start);
    while ((!deq.empty())) {
        Node cur;
        if (searchMethod == BFS) {
            cur = deq.front();
            deq.pop_front();
        } else {
            cur = deq.back();
            deq.pop_back();
        }
        if (vis[cur.c][cur.x][cur.y] != -1) {
            continue;
        }
        vis[cur.c][cur.x][cur.y] = cur.steps;
        move[cur.c][cur.x][cur.y] = cur.move;
        fromColor[cur.c][cur.x][cur.y] = cur.fromColor;
        if (mazeMap[cur.x][cur.y] == '?') {
            targetReached = true;
            break;
        }
        for (ushort dir = 0; dir < 4; dir++) {
            Node nxt(cur.x + dx[dir], cur.y + dy[dir], cur.c, cur.steps + 1, Move(dir), cur.c);
            if (isInMap(nxt)) {
                char tc = mazeMap[nxt.x][nxt.y];
                if (tc == '.') {
                } else if (tc == '#') {
                    continue;
                } else if (islower(tc) || tc == '^') {
                    nxt.c = c2n(tc);
                } else if (isupper(tc)) {
                    if (cur.c != c2n(tc)) {
                        continue;
                    }
                }
                deq.push_back(nxt);
            }
        }
    }
    if (targetReached) {
        outputSol(vis, move, fromColor);
    } else {
        outputNoSol(vis);
    }
}

inline bool MazeSolving::isValid(const Node& cur, const vector<vector<vector<int32_t>>>& vis) {
    return isInMap(cur) && vis[cur.c][cur.x][cur.y] == -1;
}

inline bool MazeSolving::isInMap(const Node& cur) {
    return cur.x >= 0 && cur.x < int32_t(height) && cur.y >= 0 && cur.y < int32_t(width);
}

void MazeSolving::outputSol(const vector<vector<vector<int32_t>>>& vis, const vector<vector<vector<Move>>>& move, const vector<vector<vector<ushort>>>& fromColor) {
    vector<Move> ansMove;
    for (ushort k = 0; k <= nColor; k++) {
        if (vis[k][target.x][target.y] != -1) {
            Node cur(target.x, target.y, k, 0, UP, 0);
            while (cur.x != start.x || cur.y != start.y || cur.c != 0) {
                // cout << cur.x << cur.y << cur.c << start.x << start.y << endl;
                ansMove.push_back(move[cur.c][cur.x][cur.y]);
                cur = Node(cur.x - dx[move[cur.c][cur.x][cur.y]], cur.y - dy[move[cur.c][cur.x][cur.y]], fromColor[cur.c][cur.x][cur.y], 0, UP, 0);
            }
        }
    }
    // cout << "Done" << endl;
    if (outputType == LIST) {
        Node cur = start;
        cout << "(^, (" << cur.x << ", " << cur.y << "))\n";
        while (!ansMove.empty()) {
            Move m = ansMove.back();
            ansMove.pop_back();
            cur = Node(cur.x + dx[m], cur.y + dy[m], cur.c, 0, UP, 0);
            char tc = mazeMap[cur.x][cur.y];
            if (islower(tc) || tc == '^') {
                cur.c = c2n(tc);
            }
            cout << "(" << n2c(cur.c) << ", (" << cur.x << ", " << cur.y << "))\n";
        }
    }
}
void MazeSolving::outputNoSol(const vector<vector<vector<int32_t>>>& vis) {
    cout << "No solution.\nDiscovered:\n";
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            bool visited = false;
            for (uint32_t k = 0; k <= nColor; k++) {
                if (vis[k][i][j] != -1) {
                    visited = true;
                    break;
                }
            }
            cout << (visited ? mazeMap[i][j] : '#');
        }
        cout << "\n";
    }
}
