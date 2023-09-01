#include <getopt.h>
#include <cctype>
#include <cstring>
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
        : nColor(0), height(0), width(0), triedSolve(false), outputType(MAP), hasStart(false), hasTarget(false) {}
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
    class Node {
       public:
        Node() {}
        Node(int32_t x, int32_t y, char c, uint32_t steps)
            : x(x), y(y), c(c), steps(steps) {}
        void updateNode(int32_t x, int32_t y, char c, uint32_t steps) {
            this->x = x;
            this->y = y;
            this->c = c;
            this->steps = steps;
        }
        int32_t x;
        int32_t y;
        char c;  // abcd...z for corresponding activated button; ^ for trap
        uint32_t steps;
        class NodeSort {
           public:
            bool operator()(const Node& n1, const Node& n2) {
                return n1.steps < n2.steps;
            }
        };
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
};

int main(int argc, char** argv) {
    try {
        ios_base::sync_with_stdio(false);
        MazeSolving maze;
        maze.getOptions(argc, argv);
        maze.readMaze();
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
    // opterr = 0;
    struct option longOpts[] = {{"queue", no_argument, nullptr, 'q'},
                                {"stack", no_argument, nullptr, 's'},
                                {"output", required_argument, nullptr, 'o'},
                                {"help", no_argument, nullptr, 'h'}};

    bool outputTypeSpecified = false;
    bool searchMethodSpecified = false;

    while ((option = getopt_long(argc, argv, "qso:h", longOpts, &optionIdx)) != -1) {
        switch (option) {
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
            case 'h':
                cout << "I'm so lazy writing a help that is already available at https://eecs281staff.github.io/p1-puzzle/#puzzle-options\n";
                exit(0);
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
                    start.updateNode(i, j, '@', 0);
                    break;
                case '?':
                    if (hasTarget) {
                        throw(Error("Error: the maze cannot have more than 1 target"));
                    }
                    hasTarget = true;
                    target.updateNode(i, j, '?', 0);
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