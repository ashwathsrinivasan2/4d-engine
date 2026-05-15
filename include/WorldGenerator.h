
#ifndef WORLD_GENERATOR
#define WORLD_GENERATOR

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <iostream>

#include "Scene.h"


class WorldGenerator {

    unsigned currVID = 0;

    int numRooms = 10;

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

    struct Room {
        glm::uvec4 minCorner;
        glm::uvec4 dimensions;
    };

    Scene* scene;
    std::string buildInstructions;

    std::vector<Room> rooms;

    std::mt19937 rng{ std::random_device{}() };

    glm::vec4 doorDimensions = glm::vec4(4.f, 6.f, 4.f, 4.f);

    float edgeSelectionProbability = 0.15f;

    unsigned worldGrid[8][8][8][8];

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

    glm::uvec4 getRandUvec4(glm::uvec4, glm::uvec4);

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

public:
    WorldGenerator(Scene*);
    void randGenerate();
};

#endif