// identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725

#include <getopt.h>
#include <cctype>
#include <cstring>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Error {
   public:
    string errMessage;
    Error() {}
    Error(const string& s)
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
    const int32_t dx[5] = {-1, 0, 1, 0, 0};
    const int32_t dy[5] = {0, 1, 0, -1, 0};
    enum SearchMethod {
        BFS,
        DFS
    };
    enum Move {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        CHANGE_COLOR
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
        Node(int32_t x, int32_t y, uint8_t c, Move move, uint8_t fromColor)
            : x(x), y(y), c(c), move(move), fromColor(fromColor) {}
        void updateNode(int32_t x, int32_t y, uint8_t c, Move move, uint8_t fromColor) {
            this->x = x;
            this->y = y;
            this->c = c;
            this->move = move;
            this->fromColor = fromColor;
        }
        int32_t x;
        int32_t y;
        uint8_t c;  // 0...26
        Move move;
        uint8_t fromColor;
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

    inline bool isInMap(const Node& cur);
    inline uint8_t c2n(char c) {
        return c == '^' ? 0 : uint8_t(islower(c) ? c - 'a' + 1 : c - 'A' + 1);
    }
    inline char n2c(uint32_t n) {
        return n == 0 ? '^' : char('a' + n - 1);
    }
    inline uint8_t encode(Move m, uint8_t c) {
        return uint8_t((m << 5) + c + 1);
    }
    inline Move decodeMove(uint8_t n) {
        return Move((n - 1) >> 5);
    }
    inline uint8_t decodeC(uint8_t n) {
        return uint8_t((n - 1) & 31);
    }
    void outputSol(vector<vector<vector<uint8_t>>>& status);
    void outputNoSol(const vector<vector<vector<uint8_t>>>& status);
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
    string st;
    getline(cin, st);
    while (st == "" || st == "\n" || (st.length() >= 2 && st[0] == '/' && st[1] == '/'))
        getline(cin, st);
    for (uint32_t i = 0; i < height; i++) {
        istringstream ss;
        if (i == 0)
            ss = istringstream(st);
        mazeMap[i].resize(width);
        for (uint32_t j = 0; j < width; j++) {
            if (((i == 0) && (!(ss >> mazeMap[i][j]))) || (i > 0 && (!(cin >> mazeMap[i][j])))) {
                throw(Error("Error: The map size does not agree with height*width"));
            }
            switch (mazeMap[i][j]) {
                case '@':
                    if (hasStart) {
                        throw(Error("Error: the maze cannot have more than 1 start"));
                    }
                    hasStart = true;
                    start.updateNode(i, j, 0, UP, 0);
                    break;
                case '?':
                    if (hasTarget) {
                        throw(Error("Error: the maze cannot have more than 1 target"));
                    }
                    hasTarget = true;
                    target.updateNode(i, j, 0, UP, 0);
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
        // if (i < height - 1)
        //     getline(cin, st);
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
    vector<vector<vector<uint8_t>>> status(nColor + 1, vector<vector<uint8_t>>(height, vector<uint8_t>(width, 0)));
    deque<Node> deq;
    deq.push_back(start);
    status[start.c][start.x][start.y] = 1;
    while ((!deq.empty())) {
        Node cur;
        if (searchMethod == BFS) {
            cur = deq.front();
            deq.pop_front();
        } else {
            cur = deq.back();
            deq.pop_back();
        }

        // cout << "-------------"
        //      << cur.x << cur.y << cur.c
        //      << endl;
        // if (mazeMap[cur.x][cur.y] == '?') {
        // targetReached = true;
        // break;
        // }
        if ((islower(mazeMap[cur.x][cur.y]) || mazeMap[cur.x][cur.y] == '^') && cur.c != c2n(mazeMap[cur.x][cur.y])) {
            Node nxt(cur.x, cur.y, c2n(mazeMap[cur.x][cur.y]), CHANGE_COLOR, cur.c);
            if (!status[nxt.c][nxt.x][nxt.y]) {
                deq.push_back(nxt);
                status[nxt.c][nxt.x][nxt.y] = encode(nxt.move, nxt.fromColor);
            }
        } else {
            for (uint8_t dir = 0; dir < 4; dir++) {
                Node nxt(cur.x + dx[dir], cur.y + dy[dir], cur.c, Move(dir), cur.c);
                if (isInMap(nxt)) {
                    char tc = mazeMap[nxt.x][nxt.y];
                    if (tc == '.') {
                    } else if (tc == '#') {
                        continue;
                    } else if (isupper(tc)) {
                        if (cur.c != c2n(tc)) {
                            continue;
                        }
                    }
                    if (!status[nxt.c][nxt.x][nxt.y]) {
                        deq.push_back(nxt);
                        status[nxt.c][nxt.x][nxt.y] = encode(nxt.move, nxt.fromColor);
                        if (mazeMap[nxt.x][nxt.y] == '?') {
                            targetReached = true;
                            break;
                        }
                    }
                    // cout << nxt.x << nxt.y << nxt.c << endl;
                }
            }
            if (targetReached)
                break;
        }
    }
    if (targetReached) {
        outputSol(status);
    } else {
        outputNoSol(status);
    }
}

inline bool MazeSolving::isInMap(const Node& cur) {
    return cur.x >= 0 && cur.x < int32_t(height) && cur.y >= 0 && cur.y < int32_t(width);
}

void MazeSolving::outputSol(vector<vector<vector<uint8_t>>>& status) {
    const uint8_t STARTING = (6 << 5) + 1, ENDING = (6 << 5) + 2, PASSING = (6 << 5) + 3;
    vector<uint8_t> ansMove;
    for (uint8_t k = 0; k <= uint8_t(nColor); k++) {
        if (status[k][target.x][target.y]) {
            Node cur(target.x, target.y, k, UP, 0);
            Move nextMove = UP;
            while (cur.x != start.x || cur.y != start.y || cur.c != 0) {
                // cout << cur.x << cur.y << cur.c << start.x << start.y << endl;
                ansMove.push_back(decodeMove(status[cur.c][cur.x][cur.y]));
                Move curMove = decodeMove(status[cur.c][cur.x][cur.y]);
                uint8_t fromC = decodeC(status[cur.c][cur.x][cur.y]);
                if (nextMove == CHANGE_COLOR) {
                    status[cur.c][cur.x][cur.y] = ENDING;
                } else if (curMove == CHANGE_COLOR) {
                    status[cur.c][cur.x][cur.y] = STARTING;
                } else {
                    status[cur.c][cur.x][cur.y] = PASSING;
                }
                nextMove = curMove;
                cur = Node(cur.x - dx[curMove], cur.y - dy[curMove], fromC, UP, 0);
            }
            status[0][start.x][start.y] = STARTING;
            break;
        }
    }
    if (outputType == LIST) {
        Node cur = start;
        cout << "(^, (" << cur.x << ", " << cur.y << "))\n";
        while (!ansMove.empty()) {
            Move m = Move(ansMove.back());
            ansMove.pop_back();
            cur = Node(cur.x + dx[m], cur.y + dy[m], cur.c, UP, 0);
            if (m == CHANGE_COLOR) {
                cur.c = c2n(mazeMap[cur.x][cur.y]);
            }
            cout << "(" << n2c(cur.c) << ", (" << cur.x << ", " << cur.y << "))\n";
        }
    } else {
        for (uint8_t k = 0; k <= uint8_t(nColor); k++) {
            cout << "// color " << n2c(k) << '\n';
            for (uint32_t i = 0; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    switch (status[k][i][j]) {
                        case STARTING:
                            cout << '@';
                            break;
                        case ENDING:
                            cout << '%';
                            break;
                        case PASSING:
                            cout << (uint32_t(target.x) == i && uint32_t(target.y) == j ? '?' : '+');
                            break;
                        default:
                            cout << (c2n(mazeMap[i][j]) == k || mazeMap[i][j] == '@' ? '.' : mazeMap[i][j]);
                    }
                }
                cout << '\n';
            }
        }
    }
}
void MazeSolving::outputNoSol(const vector<vector<vector<uint8_t>>>& status) {
    cout << "No solution.\nDiscovered:\n";
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            bool visited = false;
            for (uint32_t k = 0; k <= nColor; k++) {
                if (status[k][i][j]) {
                    visited = true;
                    break;
                }
            }
            cout << (visited ? mazeMap[i][j] : '#');
        }
        cout << "\n";
    }
}
