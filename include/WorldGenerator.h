//DIRECTIONS: 0 - x-, 1 - x+, 2 - y-, 3 - y+, 4 - z-, 5 - z+, 6 - w-, 7 - w+

//GRID (cell space):
//ranges from (0, 0, 0, 0) to (gridRes, gridRes, gridRes, gridRes)
//centered at (gridRes / 2, gridRes / 2, gridRes / 2, gridRes / 2)
//integer coordinates only

//


#ifndef WORLD_GENERATOR
#define WORLD_GENERATOR

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "Scene.h"

namespace std {
    template<> struct hash<glm::ivec4> {
        size_t operator()(const glm::ivec4& v) const {
            size_t seed = 0;
            seed ^= hash<unsigned>()(v.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash<unsigned>()(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash<unsigned>()(v.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash<unsigned>()(v.w) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}


class WorldGenerator {

    

    struct Vertex {
        glm::vec4 pos;
        unsigned vID;
        Vertex(glm::vec4 inPos, unsigned inVID) : pos(inPos), vID(inVID) {}
        bool equals(Vertex v) { return pos == v.pos; }
    };

    struct Cell {
        Vertex v0;
        Vertex v1;
        Vertex v2;
        Vertex v3;
        Cell(Vertex a, Vertex b, Vertex c, Vertex d) : v0(a), v1(b), v2(c), v3(d){}
        bool equals(Cell e) {
            Vertex original[4] = { v0, v1, v2, v3 };
            Vertex alias[4] = { e.v0, e.v1, e.v2, e.v3 };
            bool aliasFound = false;

           
            for (int i = 0; i < 4; i++) {
                aliasFound = false;
                for (int j = 0; j < 4; j++) {
                    if (original[i].equals(alias[j])) {
                        aliasFound = true;
                        break;
                    }
                }
                if (!aliasFound) return false;
            }
            return true;
        }
    };

    struct Pentachoron {
        Vertex v0;
        Vertex v1;
        Vertex v2;
        Vertex v3;
        Vertex v4;

        glm::vec4 hyperSphereCenter;
        float hyperSphereRadius;

        Pentachoron(Vertex a, Vertex b, Vertex c, Vertex d, Vertex e) : v0(a), v1(b), v2(c), v3(d), v4(e) {
        }

        bool inCircumCircle(Vertex v) {
            return glm::length(v.pos - hyperSphereCenter) <= hyperSphereRadius;
        }
    };

    struct mstEdge {
        int nodeA;
        int nodeB;
        int weight;
    };

    struct CompareCost {
        bool operator()(
            const std::tuple<int, glm::ivec4, glm::ivec4>& a,
            const std::tuple<int, glm::ivec4, glm::ivec4>& b) const {
            return std::get<0>(a) > std::get<0>(b);
        }
    };

    struct Fringe {
        std::priority_queue<
            std::tuple<int, glm::ivec4, glm::ivec4>,
            std::vector<std::tuple<int, glm::ivec4, glm::ivec4>>,
            CompareCost> fringe;
        std::unordered_set<glm::ivec4> visited;
        std::unordered_map<glm::ivec4, glm::ivec4> previous;
        std::unordered_map<glm::ivec4, int> g_costs;

        void push(int g, int h, glm::ivec4 pos, glm::ivec4 prev) {

            if (!visited.contains(pos) && (!g_costs.contains(pos) || g_costs[pos] > g)) {
                g_costs[pos] = g;
                fringe.push({ g + h, pos, prev });
            }
        }

        bool pop(int& gCost, glm::ivec4& pos, glm::ivec4& prev) {
            auto [c, position, prevPos] = fringe.top();
            gCost = g_costs[position];
            pos = position;
            prev = prevPos;
            fringe.pop();

            if (!visited.contains(pos)) {
                visited.insert(pos);
                previous[pos] = prev;
                return true;
            }
            return false;
        }

        bool empty() { return fringe.empty(); }

        std::vector<glm::ivec4> getPath(glm::ivec4 start, glm::ivec4 goal) {
            std::vector<glm::ivec4> path;


            int currIter = 0;
            int maxIter = 1000;
            previous[start] = start;
            while (goal != start) {
                currIter++;
                if (currIter > maxIter) throw std::runtime_error("infinite loop");
                path.push_back(goal);
                
                goal = previous[goal];
            }

            path.push_back(goal);

            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    struct Room {
        glm::ivec4 minCorner;
        glm::ivec4 dimensions;
    };

    struct Corridor {
        glm::ivec4 start;
        glm::ivec4 end;
        int direction;
        int numCornerEndpoints;

        Corridor(glm::ivec4 s, glm::ivec4 g, int dir, int corners) :
            start(s), end(g), direction(dir), numCornerEndpoints(corners){ }
    };

    struct CorridorCorner {
        std::vector<int> walledDirections;
        glm::ivec4 position;

        CorridorCorner(std::vector<int> dir, glm::ivec4 pos) : walledDirections(dir), position(pos) {}
    };

    Scene* scene;

    std::vector<Room> rooms;
    std::vector<Corridor> corridors;
    std::vector<CorridorCorner> corridorCorners;
    std::unordered_map<glm::ivec4, std::vector<int>> doorways;

    std::mt19937 rng{ std::random_device{}() };

    unsigned currVID = 0;

    std::vector<std::vector<std::vector<std::vector<unsigned>>>> worldGrid;

    float gridScale = 5.f;
    unsigned gridRes = 8;
    int numRooms = 10;
    float edgeSelectionProbability = 0.15f;

    glm::vec4 doorDimensions = glm::vec4(1.f);
    glm::ivec4 minRoomDim = glm::ivec4(1, 1, 1, 1);
    glm::ivec4 maxRoomDim = glm::ivec4(3, 3, 3, 3);



    int getOrientedCube(int direction, glm::vec3 color = glm::vec3(1.f));

    bool causesOverlap(Room);

    inline void printVec(std::string name, glm::vec4 vec) {
        std::cout << name << ": {";
        for (int i = 0; i < 4; i++) {
            std::cout << vec[i];
            if (i != 4) std::cout << " ";
        }
        std::cout << "}" << std::endl;
    }

    glm::ivec4 getRandIvec4(glm::ivec4, glm::ivec4);

    unsigned getGridVal(glm::ivec4);

    //builder functions
    int wallWithDoor(glm::vec4, glm::vec3, int);
    int corridorCorner(std::vector<int>);
    int corridor(float, int);
    int createRoom(glm::vec4);

    //triangulation functions
    //inspired by https://www.gorillasun.de/blog/bowyer-watson-algorithm-for-delaunay-triangulation/
    Pentachoron getCircumHyperSphere(Pentachoron p);
    std::vector<std::vector<bool>> pentachoronizeRooms();
    std::vector<Pentachoron> addVertex(Vertex v, std::vector<Pentachoron>);

    //gaussian elimination helpers
    std::vector<float> rowAdd(std::vector<float>, std::vector<float>);
    std::vector<float> rowMultiply(std::vector<float>, float);
    std::vector<float> solveSystem(std::vector<std::vector<float>>, std::vector<float>);

    //mst functions
    int find(int, std::vector<int>);
    bool unionElements(int, int, std::vector<int>&, std::vector<int>&);
    std::vector<std::vector<bool>> createMST(std::vector<std::vector<bool>>);

    //pathfinding functions
    std::vector<glm::ivec4> aStar(int roomA, int roomB);
    bool checkAdjacentCellsMatch(glm::ivec4, glm::ivec4);
    bool checkIfStraight(glm::ivec4);
    std::vector<int> getWalledDirections(glm::ivec4);

    

    int createMeshes();

    template<typename T>
    inline bool contains(std::vector<T> list, T value) {
        for (int i = 0; i < list.size(); i++) {
            if (list[i] == value) {
                return true;
            }
        }
        return false;
    }

    inline glm::vec4 getTranslate(glm::vec4 scalar, glm::ivec4 startCorner, glm::ivec4 currMin, bool posSide, int side) {
        scalar[side] = 0.f;
        glm::vec4 translate = scalar / 2.f      //move section's minCorner to world origin
            - glm::vec4((float)gridRes / 2.f)   //move section's minCorner to grid origin (grid's minCorner)
            + glm::vec4(startCorner)     //move section's minCorner to room's minCorner
            + glm::vec4(currMin);                  //move section's minCorner to currMin's minCorner
        translate[side] += (posSide ? 1.f : 0.f);
        return translate;
    }

    void testGrid() {
        rooms = {};
        corridors = {};
        corridorCorners = {};

        glm::ivec4 minCorner(2, 2, 2, 2);
        glm::ivec4 dim(4, 4, 4, 4);

        Room newRoom(minCorner, dim);
        rooms.push_back(newRoom);
        for (int i = 0; i < gridRes; i++) {
            for (int j = 0; j < gridRes; j++) {
                for (int k = 0; k < gridRes; k++) {
                    for (int l = 0; l < gridRes; l++) {
                        worldGrid[i][j][k][l] = 0;
                    }
                }
            }
        }

        for (int i = minCorner.x; i < minCorner.x + dim.x; i++) {
            for (int j = minCorner.y; j < minCorner.y + dim.y; j++) {
                for (int k = minCorner.z; k < minCorner.z + dim.z; k++) {
                    for (int l = minCorner.w; l < minCorner.w + dim.w; l++) {
                        worldGrid[i][j][k][l] = 1;
                    }
                }
            }
        }

    }

public:
    WorldGenerator(Scene*);
    int randGenerate();
    glm::vec4 getSpawnPos();
};

#endif