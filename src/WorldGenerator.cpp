#include "WorldGenerator.h"
#include <algorithm>

WorldGenerator::WorldGenerator(Scene* scene) : scene(scene){
}

glm::uvec4 WorldGenerator::getRandUvec4(glm::uvec4 min, glm::uvec4 max) {
    glm::uvec4 random;
    for (int i = 0; i < 4; i++) {
        std::uniform_int_distribution<int> dist(min[i], max[i]);
        random[i] = dist(rng);
    }
    return random;
}

bool WorldGenerator::causesOverlap(Room newRoom) {
    for (int i = 0; i < rooms.size(); i++) {
        bool overlaps = true;
        for (int j = 0; j < 4; j++) {
            if (newRoom.minCorner[j] + newRoom.dimensions[j] <= rooms[i].minCorner[j] ||
                newRoom.minCorner[j] >= rooms[i].minCorner[j] + rooms[i].dimensions[j]) {
                overlaps = false;
                break;
            }
        }
        if (overlaps) return true;
    }
    return false;
}

void WorldGenerator::randGenerate() {

    //initialize worldGrid
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    worldGrid[i][j][k][l] = 0;
                }
            }
        }
    }

    //generate rooms
    for (int i = 0; i < numRooms; i++) {
        bool overlap = true;
        Room newRoom;
        while (overlap) {
            newRoom.dimensions = getRandUvec4(glm::uvec4(1, 1, 1, 1), glm::uvec4(3, 3, 3, 3));
            newRoom.minCorner = getRandUvec4(glm::uvec4(0), glm::uvec4(7) - newRoom.dimensions);
            
            overlap = causesOverlap(newRoom);
        }
        rooms.push_back(newRoom);
    }

    //add rooms to grid
    for (int i = 0; i < rooms.size(); i++) {
        glm::uvec4 minCorner = glm::uvec4(rooms[i].minCorner);
        glm::uvec4 max = glm::uvec4(rooms[i].dimensions) + minCorner;
        for (int j = minCorner.x; j < max.x; j++) {
            for (int k = minCorner.y; k < max.y; k++) {
                for (int l = minCorner.z; l < max.z; l++) {
                    for (int m = minCorner.w; m < max.w; m++) {
                        worldGrid[j][k][l][m] = 1;
                    }
                }
            }
        }
    }

    std::vector<std::vector<bool>> adjMatrix = pentachoronizeRooms();

    //create MST and add back some edges
    std::vector<std::vector<bool>> mst = createMST(adjMatrix);

    for (int i = 0; i < adjMatrix.size(); i++) {
        for (int j = i + 1; j < adjMatrix[i].size(); j++) {
            if (adjMatrix[i][j] && !mst[i][j]) {
                std::uniform_real_distribution<float> dist(0.f, 1.f);
                float randVal = dist(rng);
                if (randVal < edgeSelectionProbability) {
                    mst[i][j] = true;
                    mst[j][i] = true;
                }
            }
        }
    }

    for (int i = 0; i < mst.size(); i++) {
        for (int j = i + 1; j < mst[i].size(); j++) {
            if (mst[i][j]) {
                std::cout << i << "-->" << j << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

int WorldGenerator::find(int i, std::vector<int> parents) {
    return parents[i] == i ? i : find(parents[i], parents);
}

bool WorldGenerator::unionElements(int a, int b, std::vector<int>& parents, std::vector<int>& sizes) {
    int pA = find(a, parents);
    int pB = find(b, parents);
    if (pB == pA) return false;

    if (sizes[pA] > sizes[pB]) {
        parents[pB] = pA;
        sizes[pA] += sizes[pB];
    }
    else {
        parents[pA] = pB;
        sizes[pB] += sizes[pA];
    }

    return true;
}

std::vector<std::vector<bool>> WorldGenerator::createMST(std::vector<std::vector<bool>> adjMatrix) {
    

    std::vector<mstEdge> edges;

    //fill edges vector from adjMatrix
    for (int i = 0; i < adjMatrix.size(); i++) {
        for (int j = i + 1; j < adjMatrix[i].size(); j++) {
            if (adjMatrix[i][j]) {
                mstEdge newEdge;
                newEdge.nodeA = i;
                newEdge.nodeB = j;
                glm::uvec4 centerA = rooms[i].minCorner + rooms[i].dimensions / 2u;
                glm::uvec4 centerB = rooms[j].minCorner + rooms[j].dimensions / 2u;
                glm::vec4 diff = glm::vec4(centerB) - glm::vec4(centerA);

                newEdge.weight = abs(diff.x) + abs(diff.y) + abs(diff.z) + abs(diff.w);
                edges.push_back(newEdge);
            }
        }
    }
    std::sort(edges.begin(), edges.end(), [](const mstEdge& a, const mstEdge& b) {return a.weight < b.weight;});

    std::vector<int> parents;
    std::vector<int> sizes;
    
    for (int i = 0; i < rooms.size(); i++) {
        parents.push_back(i);
        sizes.push_back(1);
    }

    std::vector<std::vector<bool>> includedEdges;
    for (int i = 0; i < rooms.size(); i++) {
        std::vector<bool> row;
        for (int j = 0; j < rooms.size(); j++) {
            row.push_back(false);
        }
        includedEdges.push_back(row);
    }

    for (int i = 0; i < edges.size(); i++) {
        if (unionElements(edges[i].nodeA, edges[i].nodeB, parents, sizes)) {
            includedEdges[edges[i].nodeA][edges[i].nodeB] = true;
            includedEdges[edges[i].nodeB][edges[i].nodeA] = true;
        }
    } 

    return includedEdges;
}

std::vector<float> WorldGenerator::rowAdd(std::vector<float> a, std::vector<float> b) {
    std::vector<float> res;
    for (int i = 0; i < a.size(); i++) {
        res.push_back(a[i] + b[i]);
    }
    return res;
}

std::vector<float> WorldGenerator::rowMultiply(std::vector<float> a, float b) {
    std::vector<float> res;
    for (int i = 0; i < a.size(); i++) {
        res.push_back(a[i] * b);
    }
    return res;
}

std::vector<float> WorldGenerator::solveSystem(std::vector<std::vector<float>> A, std::vector<float> b) {

    for (int i = 0; i < b.size(); i++) {

        int pivot = i;
        for (int k = i + 1; k < b.size(); k++)
            if (std::abs(A[k][i]) > std::abs(A[pivot][i])) pivot = k;

        // swap rows
        std::swap(A[i], A[pivot]);
        std::swap(b[i], b[pivot]);

        float norm = 1.f / A[i][i];
        A[i] = rowMultiply(A[i], norm);
        b[i] *= norm;
        for (int j = 0; j < b.size(); j++) {
            if (i != j) {
                float mul = -1.f * A[j][i];
                std::vector<float> temp = rowMultiply(A[i], mul);
                A[j] = rowAdd(temp, A[j]);
                b[j] += b[i] * mul;
            }
        }
    }

    return b;
}

WorldGenerator::Pentachoron WorldGenerator::getCircumHyperSphere(Pentachoron p) {

    std::vector<std::vector<float>> A;
    std::vector<float> b;
    Vertex vertices[5] = { p.v0, p.v1, p.v2, p.v3, p.v4 };

    //set up A and b
    for (int i = 0; i < 5; i++) {
        std::vector<float> row;
        Vertex v = vertices[i];
        float res = 0.f;
        for (int j = 0; j < 4; j++) {
            row.push_back(-2.f * vertices[i].pos[j]);
            res += vertices[i].pos[j] * vertices[i].pos[j];
        }
        row.push_back(1.f);
        res *= -1.f;
        b.push_back(res);
        A.push_back(row);
    }


    b = solveSystem(A, b);


    p.hyperSphereCenter = glm::vec4(b[0], b[1], b[2], b[3]);
    p.hyperSphereRadius = sqrtf(b[0] * b[0] + b[1] * b[1] + b[2] * b[2] + b[3] * b[3] - b[4]);
  
    return p;
}

std::vector<std::vector<bool>> WorldGenerator::pentachoronizeRooms() {
    //create super pentachoron
    glm::vec4 min(999.f);
    glm::vec4 max(-999.f);

    for (int i = 0; i < rooms.size(); i++) {
        glm::vec4 currMin = glm::vec4(rooms[i].minCorner);
        glm::vec4 currMax = currMin + glm::vec4(rooms[i].dimensions);

        for (int j = 0; j < 4; j++) {
            if (currMin[j] < min[j]) {
                min[j] = currMin[j];
            }
            if (currMax[j] > max[j]) {
                max[j] = currMax[j];
            }
        }
    }

    std::vector<Vertex> vertices;
    for (int i = 0; i < rooms.size(); i++) {
        Vertex newVertex(glm::vec4(rooms[i].minCorner) + glm::vec4(rooms[i].dimensions) / 2.f, i);
        vertices.push_back(newVertex);
    }

    glm::vec4 diff = max - min;
    float padding = 0.1f;

    glm::vec4 p0 = min - glm::vec4(padding);
    glm::vec4 p1 = p0;
    glm::vec4 p2 = p0;
    glm::vec4 p3 = p0;
    glm::vec4 p4 = p0;
    
    p1.x += diff.x * 5.f;
    p2.y += diff.y * 5.f;
    p3.z += diff.z * 5.f;
    p4.w += diff.w * 5.f;

    Pentachoron super(Vertex(p0, 5), Vertex(p1, 6), Vertex(p2, 7), Vertex(p3, 8), Vertex(p4, 9));
    super = getCircumHyperSphere(super);

    //add vertices to triangulation
    std::vector<Pentachoron> pChorons = { super };
    for (int i = 0; i < vertices.size(); i++) {
        pChorons = addVertex(vertices[i], pChorons);
    }

    //remove original super pentachoron vertices
    for (int i = 0; i < pChorons.size(); i++) {
        Pentachoron curr = pChorons[i];

        Vertex currVertices[5] = { curr.v0, curr.v1, curr.v2, curr.v3, curr.v4 };
        Vertex superVertices[5] = { super.v0, super.v1, super.v2, super.v3, super.v4 };

        bool check = true;
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                if (currVertices[j].equals(superVertices[k])) {
                    check = false;
                    break;
                }
            }
            if (!check) break;
        }
        if (!check) {
            pChorons.erase(pChorons.begin() + i);
            i--;
        }
    }

    //create adjacency matrix
    std::vector<std::vector<bool>> connections;

    for (int i = 0; i < rooms.size(); i++) {
        std::vector<bool> row;
        for (int j = 0; j < rooms.size(); j++) {
            row.push_back(false);
        }
        connections.push_back(row);
    }


    for (int i = 0; i < pChorons.size(); i++) {
        Pentachoron curr = pChorons[i];
        Vertex vertices[5] = { curr.v0, curr.v1, curr.v2, curr.v3, curr.v4 };
        for (int j = 0; j < 5; j++) {
            for (int k = j + 1; k < 5; k++) {
                connections[vertices[j].vID][vertices[k].vID] = true;
                connections[vertices[k].vID][vertices[j].vID] = true;
            }
        }
    }

    return connections;
}

std::vector<WorldGenerator::Pentachoron> WorldGenerator::addVertex(Vertex v, std::vector<Pentachoron> pChorons) {
    std::vector<Cell> cells;
    for (int i = 0; i < pChorons.size(); i++) {
        Pentachoron curr = pChorons[i];
        if (curr.inCircumCircle(v)) {
            Cell a(curr.v0, curr.v1, curr.v2, curr.v3);
            Cell b(curr.v1, curr.v2, curr.v3, curr.v4);
            Cell c(curr.v2, curr.v3, curr.v4, curr.v0);
            Cell d(curr.v3, curr.v4, curr.v0, curr.v1);
            Cell e(curr.v4, curr.v0, curr.v1, curr.v2);

            cells.push_back(a);
            cells.push_back(b);
            cells.push_back(c);
            cells.push_back(d);
            cells.push_back(e);

            pChorons.erase(pChorons.begin() + i);
            i--;
        }
    }

    //remove duplicate cells
    for (int i = 0; i < cells.size(); i++) {
        for (int j = i + 1; j < cells.size(); j++) {
            if (cells[i].equals(cells[j])) {
                cells.erase(cells.begin() + j);
                cells.erase(cells.begin() + i);
                i--;
                break;
            }
        }
    }

    for (int i = 0; i < cells.size(); i++) {
        Pentachoron newPentachoron(v, cells[i].v0, cells[i].v1, cells[i].v2, cells[i].v3);
        newPentachoron = getCircumHyperSphere(newPentachoron);
        pChorons.push_back(newPentachoron);
    }

    return pChorons;
}

int WorldGenerator::getOrientedCube(int direction) {
    int cube = scene->Cube();

    int planeID = 0;
    float sign = 1.f;
    
    switch (direction) {
    case 0:
        planeID = 2;
        sign = -1.f;
        break;
    case 1:
        planeID = 2;
        sign = 1.f;
        break;
    case 2:
        planeID = 4;
        sign = -1.f;
        break;
    case 3:
        planeID = 4;
        sign = 1.f;
        break;
    case 4:
        planeID = 5;
        sign = -1.f;
        break;
    case 5:
        planeID = 5;
        sign = 1.f;
        break;
    case 6:
        planeID = 5;
        sign = 0.f;
        break;
    default:
        return cube;
    }

    if (sign != 0.f) {
        scene->rotate(cube, planeID, sign * glm::pi<float>() / 2.f);
    }
    else {
        scene->rotate(cube, planeID, glm::pi<float>());
    }
    return cube;
}

int WorldGenerator::wallWithDoor(glm::vec4 dim, glm::vec2 doorPosition) {

    int minIndex = 0;
    for (int i = 1; i < 4; i++) {
        if (dim[i] < dim[minIndex]) {
            minIndex = i;
        }
    }

    int aIndex = 1;
    int bIndex = 1;

    for (int i = 0; i < 4; i++) {
        if (i != 1 && i != minIndex) {
            if (aIndex == 1) {
                aIndex = i;
            }
            else {
                bIndex = i;
            }
        }
    }
    std::cout << minIndex << ", " << aIndex << ", " << bIndex << std::endl;

    glm::vec4 doorDim = doorDimensions;
    doorDim[minIndex] = dim[minIndex];
    glm::vec4 doorPos(0.f);
    doorPos[aIndex] = doorPosition.x;
    doorPos[bIndex] = doorPosition.y;
    doorPos[1] = doorDim[1] / 2.f - dim[1] / 2.f;

    int aNeg = scene->Tesseract(glm::vec4(1.f), false);
    int aPos = scene->Tesseract(glm::vec4(1.f), false);
    int yPos = scene->Tesseract(glm::vec4(1.f), false);
    int bNeg = scene->Tesseract(glm::vec4(1.f), false);
    int bPos = scene->Tesseract(glm::vec4(1.f), false);



    glm::vec4 aNegScalar = dim;
    aNegScalar[aIndex] = doorPos[aIndex] + dim[aIndex] / 2.f - doorDim[aIndex] / 2.f;

    glm::vec4 aPosScalar = dim;
    aPosScalar[aIndex] = dim[aIndex] - (doorPos[aIndex] + dim[aIndex] / 2.f + doorDim[aIndex] / 2.f);

    glm::vec4 yScalar = doorDim;
    yScalar[1] = dim.y - doorDim.y;

    glm::vec4 bNegScalar = doorDim;
    bNegScalar[bIndex] = doorPos[bIndex] + dim[bIndex] / 2.f - doorDim[bIndex] / 2.f;
    bNegScalar[1] = dim.y;

    glm::vec4 bPosScalar = doorDim;
    bPosScalar[bIndex] = dim[bIndex] - (doorPos[bIndex] + dim[bIndex] / 2.f + doorDim[bIndex] / 2.f);
    bPosScalar[1] = dim.y;



    glm::vec4 aNegTranslate(0.f);
    aNegTranslate[aIndex] = doorPos[aIndex] - (doorDim[aIndex] / 2.f + aNegScalar[aIndex] / 2.f);

    glm::vec4 aPosTranslate(0.f);
    aPosTranslate[aIndex] = doorPos[aIndex] + doorDim[aIndex] / 2.f + aPosScalar[aIndex] / 2.f;

    glm::vec4 yTranslate = doorPos;
    yTranslate[1] += doorDim[1] / 2.f + yScalar[1] / 2.f;

    glm::vec4 bNegTranslate = doorPos;
    bNegTranslate[bIndex] -= doorDim[bIndex] / 2.f + bNegScalar[bIndex] / 2.f;
    bNegTranslate[1] = 0.f;

    glm::vec4 bPosTranslate = doorPos;
    bPosTranslate[bIndex] += doorDim[bIndex] / 2.f + bPosScalar[bIndex] / 2.f;
    bPosTranslate[1] = 0.f;

    std::vector<int> sections = { aNeg, aPos, bNeg, bPos, yPos };
    std::vector<std::string> names = { "aNeg", "aPos", "bNeg", "bPos", "y" };
    std::vector<glm::vec4> translates = { aNegTranslate, aPosTranslate, bNegTranslate, bPosTranslate, yTranslate };
    std::vector<glm::vec4> scales = { aNegScalar, aPosScalar, bNegScalar, bPosScalar, yScalar };

    for (int i = 0; i < 5; i++) {
        scene->translate(sections[i], translates[i]);
        scene->scale(sections[i], scales[i]);
        printVec(names[i] + "Translate", translates[i]);
        printVec(names[i] + "Scale", scales[i]);
    }


    return scene->groupEntities(sections);
}

int WorldGenerator::corridorCorner(int fromDir, int toDir) {
    std::vector<int> walls;
    float thickness = 1.f;

    for (int i = 0; i < 6; i++) {
        walls.push_back(scene->Tesseract());
    }

    glm::vec4 dim = doorDimensions;

    int currIndex = 0;
    for (int i = 0; i < 8; i++) {
        if (i != fromDir && i != toDir) {
            glm::vec4 scale = dim;
            scale[i / 2] = thickness;
            glm::vec4 translate(0.f);
            translate[i / 2] = (float)((i % 2) * 2 - 1) * (dim[i / 2] / 2.f + thickness / 2.f);
            scene->scale(walls[currIndex], scale);
            scene->translate(walls[currIndex], translate);
            currIndex++;
        }
    }

    return scene->groupEntities(walls);
}

int WorldGenerator::corridor(float length, int directionDim) {
    std::vector<int> walls;
    for (int i = 0; i < 8; i++) {
        if (i / 2 != directionDim) {
            walls.push_back(getOrientedCube(i));
        }
    }

    float currIndex = 0;
    for (int i = 0; i < 8; i++) {
        if (i / 2 != directionDim) {
            glm::vec4 translate(0.f);
            translate[i / 2] = -((float)(i % 2) - 0.5f);
            scene->translate(walls[currIndex], translate);
            currIndex++;
        }
    }

    glm::vec4 dim = doorDimensions;
    dim[directionDim] = length;
    int room = scene->groupEntities(walls);
    scene->scale(room, dim);
    return room;
}

int WorldGenerator::createRoom(glm::vec4 dim) {
    std::vector<int> walls;
    for (int i = 0; i < 8; i++) {
        walls.push_back(getOrientedCube(i));
    }

    for (int i = 0; i < 8; i++) {
        glm::vec4 translate(0.f);
        translate[i / 2] = -((float)(i % 2) - 0.5f);
        scene->translate(walls[i], translate);
    }


    int room = scene->groupEntities(walls);
    scene->scale(room, dim);
    return room;
}