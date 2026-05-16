
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

            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    struct Room {
        glm::ivec4 minCorner;
        glm::ivec4 dimensions;
    };

    Scene* scene;
    std::string buildInstructions;

    std::vector<Room> rooms;

    std::mt19937 rng{ std::random_device{}() };

    glm::vec4 doorDimensions = glm::vec4(4.f, 6.f, 4.f, 4.f);

    glm::ivec4 minRoomDim = glm::ivec4(3, 3, 3, 2);
    glm::ivec4 maxRoomDim = glm::ivec4(10, 10, 10, 10);

    unsigned currVID = 0;

    int numRooms = 50;

    float edgeSelectionProbability = 0.15f;

    std::vector<std::vector<std::vector<std::vector<unsigned>>>> worldGrid;
    float gridScale = 0.25f;
    unsigned gridRes = 32;

    int getOrientedCube(int);
    void parseBuildInstructions();

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

    unsigned getGridVal(glm::ivec4 index) {
        return worldGrid[index.x][index.y][index.z][index.w];
    }

    //builder functions
    int wallWithDoor(glm::vec4, glm::vec2);
    int corridorCorner(int, int);
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

public:
    WorldGenerator(Scene*);
    void randGenerate();
};

#endif