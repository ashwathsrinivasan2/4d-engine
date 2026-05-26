#include "WorldGenerator.h"
#include <algorithm>
#include <queue>
#include <map>

WorldGenerator::WorldGenerator(Scene* scene) : scene(scene){

    worldGrid.resize(gridRes);
    for (int i = 0; i < gridRes; i++) {
        worldGrid[i].resize(gridRes);
        for (int j = 0; j < gridRes; j++) {
            worldGrid[i][j].resize(gridRes);
            for (int k = 0; k < gridRes; k++) {
                worldGrid[i][j][k].resize(gridRes);
                for (int l = 0; l < gridRes; l++) {
                    worldGrid[i][j][k][l] = 0;
                }
            }
        }
    }

}

glm::ivec4 WorldGenerator::getRandIvec4(glm::ivec4 min, glm::ivec4 max) {
    glm::ivec4 random;
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
            if (newRoom.minCorner[j] + newRoom.dimensions[j] < rooms[i].minCorner[j] ||
                newRoom.minCorner[j] > rooms[i].minCorner[j] + rooms[i].dimensions[j]) {
                overlaps = false;
                break;
            }
        }
        if (overlaps) return true;
    }
    return false;
}

unsigned WorldGenerator::getGridVal(glm::ivec4 index) {
    for (int i = 0; i < 4; i++) {
        if (index[i] < 0 || index[i] >= gridRes) {
            return -1;
        }
    }
    return worldGrid[index.x][index.y][index.z][index.w];
}

glm::vec4 WorldGenerator::getSpawnPos() {
    int currMin = 0;
    for (int i = 1; i < rooms.size(); i++) {
        if (glm::length(glm::vec4(rooms[i].minCorner)) < glm::length(glm::vec4(rooms[currMin].minCorner))) {
            currMin = i;
        }
    }

    glm::vec4 spawnPos = glm::vec4(rooms[currMin].minCorner) + glm::vec4(rooms[currMin].dimensions) / 2.f;
    spawnPos -= glm::vec4((float)gridRes / 2.f - 0.5f);
    spawnPos *= gridScale;
    return spawnPos;

}

int WorldGenerator::randGenerate() {

    //initialize worldGrid
    

    //generate rooms
    for (int i = 0; i < numRooms; i++) {
        bool overlap = true;
        Room newRoom;
        while (overlap) {
            newRoom.dimensions = getRandIvec4(minRoomDim, maxRoomDim);
            newRoom.minCorner = getRandIvec4(glm::ivec4(0), glm::ivec4(gridRes - 1) - newRoom.dimensions);
            
            overlap = causesOverlap(newRoom);
        }
        rooms.push_back(newRoom);
    }

    //add rooms to grid
    for (int i = 0; i < rooms.size(); i++) {
        glm::ivec4 minCorner = glm::ivec4(rooms[i].minCorner);
        glm::ivec4 max = glm::ivec4(rooms[i].dimensions) + minCorner;
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

    std::vector<std::vector<glm::ivec4>> paths;

    //aStar pathfinding to create corridors
    for (int i = 0; i < mst.size(); i++) {
        for (int j = i + 1; j < mst[i].size(); j++) {
            if (mst[i][j]) {
                glm::ivec4 min = rooms[j].minCorner;
                glm::ivec4 dim = rooms[j].dimensions;

                //temporarily set goal room cells to 3
                for (int k = min.x; k < min.x + dim.x; k++) {
                    for (int l = min.y; l < min.y + dim.y; l++) {
                        for (int m = min.z; m < min.z + dim.z; m++) {
                            for (int n = min.w; n < min.w + dim.w; n++) {
                                if (worldGrid[k][l][m][n] == 1) {
                                    worldGrid[k][l][m][n] = 3;
                                }
                            }
                        }
                    }
                }

                
                std::vector<glm::ivec4> path = aStar(i, j);

                paths.push_back(path);

                //add path to worldGrid
                int start = 999;
                int end = 0;
                for (int k = 0; k < path.size(); k++) {
                    if (getGridVal(path[k]) == 0 || getGridVal(path[k]) == 2) {
                        if (k < start) start = k;
                        if (k > end) end = k;
                        worldGrid[path[k].x][path[k].y][path[k].z][path[k].w] = 2;
                    }
                }
                start--;

                

                //reset goal room cells to 1
                for (int k = min.x; k < min.x + dim.x; k++) {
                    for (int l = min.y; l < min.y + dim.y; l++) {
                        for (int m = min.z; m < min.z + dim.z; m++) {
                            for (int n = min.w; n < min.w + dim.w; n++) {
                                if (worldGrid[k][l][m][n] == 3) {
                                    worldGrid[k][l][m][n] = 1;
                                }
                            }
                        }
                    }
                }

                //mark doorways and get opening direction
                worldGrid[path[start].x][path[start].y][path[start].z][path[start].w] = 4;
                worldGrid[path[end + 1].x][path[end + 1].y][path[end + 1].z][path[end + 1].w] = 4;

                glm::ivec4 startDirection = path[start + 1] - path[start];
                int startDir;
                glm::ivec4 endDirection = path[end] - path[end + 1];
                int endDir;
                for (int k = 0; k < 4; k++) {
                    if (startDirection[k] != 0) {
                        startDir = k * 2 + (startDirection[k] + 1) / 2;
                        break;
                    }
                }

                for (int k = 0; k < 4; k++) {
                    if (endDirection[k] != 0) {
                        endDir = k * 2 + (endDirection[k] + 1) / 2;
                        break;
                    }
                }

                doorways[path[start]].push_back(startDir);
                doorways[path[end + 1]].push_back(endDir);

               
            }
        }
    }

    //get corridors from path

    //go through each path
    for (int i = 0; i < paths.size(); i++) {

        std::vector<glm::ivec4> path = paths[i];

        //get the first and last corridor cells on path
        int start = 999;
        int end = 0;
        for (int k = 0; k < path.size(); k++) {
            if (getGridVal(path[k]) == 2) {
                if (k < start) start = k;
                if (k > end) end = k;
            }
        }

        glm::ivec4 currStart = path[start];
        glm::ivec4 currEnd = currStart;
        
        //take care of all corridor corners at beginning of path
        while (start <= end && !checkIfStraight(currStart)) {
            CorridorCorner newCorner(getWalledDirections(currStart), currStart);

            bool duplicate = false;
            for (int i = 0; i < corridorCorners.size(); i++) {
                if (corridorCorners[i].position == currStart) {
                    duplicate = true;
                }
            }

            if (!duplicate) {
                corridorCorners.push_back(newCorner);
            }
            start++;
            currStart = path[start];
            currEnd = currStart;
        }

        for (int j = start; j <= end; j++) {
            
            if (checkIfStraight(path[j + 1])) {
                currEnd = path[j + 1];
            }
            else {

                //get direction of corridor segment
                int dir;
                if (path[j].x != path[j + 1].x) {
                    dir = 0;
                }
                else if (path[j].y != path[j + 1].y) {
                    dir = 1;
                }
                else if (path[j].z != path[j + 1].z) {
                    dir = 2;
                }
                else if (path[j].w != path[j + 1].w) {
                    dir = 3;
                }
                else {
                    throw std::runtime_error("issue here");
                }

                //add corridor segment
                int numCornerEndpoints = 0;
                Corridor newCorridor(currStart, currEnd, dir, numCornerEndpoints);
                corridors.push_back(newCorridor);
                
                //add corner after corridor segment (if not already added)
                if (getGridVal(path[j + 1]) != 1 && getGridVal(path[j + 1]) != 4) {
                    CorridorCorner newCorner(getWalledDirections(path[j + 1]), path[j + 1]);

                    bool duplicate = false;
                    for (int i = 0; i < corridorCorners.size(); i++) {
                        if (corridorCorners[i].position == path[j + 1]) {
                            duplicate = true;
                        }
                    }

                    if (!duplicate) {
                        corridorCorners.push_back(newCorner);
                    }
                }

                //check for more corners
                if (j + 2 <= end) {
                    currStart = path[j + 2];
                    currEnd = currStart;
                    j++;
                    while (j + 2 <= end && !checkIfStraight(currStart)) {
                        if (getGridVal(currStart) != 1 && getGridVal(currStart) != 4) {
                            CorridorCorner newCorner(getWalledDirections(currStart), currStart);
                            bool duplicate = false;
                            for (int i = 0; i < corridorCorners.size(); i++) {
                                if (corridorCorners[i].position == path[j + 1]) {
                                    duplicate = true;
                                }
                            }

                            if (!duplicate) {
                                corridorCorners.push_back(newCorner);
                            }
                        }
                        currStart = path[j + 2];
                        currEnd = currStart;
                        j++;
                    }
                }

                if (j + 2 > end) {
                    CorridorCorner newCorner(getWalledDirections(currStart), currStart);
                    bool duplicate = false;
                    for (int i = 0; i < corridorCorners.size(); i++) {
                        if (corridorCorners[i].position == path[j + 1]) {
                            duplicate = true;
                        }
                    }

                    if (!duplicate) {
                        corridorCorners.push_back(newCorner);
                    }

                    break;
                }
            }
        }
    }

    return createMeshes();
}

std::vector<int> WorldGenerator::getWalledDirections(glm::ivec4 pos) {
    glm::ivec4 offsets[] = {
    {-1,0,0,0},{1,0,0,0},
    {0,-1,0,0},{0,1,0,0},
    {0,0,-1,0},{0,0,1,0},
    {0,0,0,-1},{0,0,0,1}
    };

    
    std::vector<int> walledDirections;

    for (int i = 0; i < 8; i++) {
        int flipped = i % 2 == 0 ? i + 1 : i - 1;;
        if (getGridVal(pos + offsets[i]) == 0 ||
            getGridVal(pos + offsets[i]) == 1 || 
            getGridVal(pos + offsets[i]) == -1 ||
            (getGridVal(pos + offsets[i]) == 4 && !contains(doorways[pos + offsets[i]], flipped) )) walledDirections.push_back(i);
    }

    return walledDirections;
}

bool WorldGenerator::checkIfStraight(glm::ivec4 pos) {
    glm::ivec4 offsets[] = {
    {-1,0,0,0},{1,0,0,0},
    {0,-1,0,0},{0,1,0,0},
    {0,0,-1,0},{0,0,1,0},
    {0,0,0,-1},{0,0,0,1}
    };

    std::vector<bool> adjCorridors;
    for (int i = 0; i < 8; i++) {
        adjCorridors.push_back(false);
    }

    int count = 0;

    for (int i = 0; i < 8; i++) {
        int adjCellType = getGridVal(pos + offsets[i]);
        if (adjCellType == 2 || adjCellType == 4) {
            adjCorridors[i] = true;
            count++;
        }
    }

    if (count == 2 && 
        (
        (adjCorridors[0] && adjCorridors[1]) ||
        (adjCorridors[2] && adjCorridors[3]) ||
        (adjCorridors[4] && adjCorridors[5]) ||
        (adjCorridors[6] && adjCorridors[7])
        )
        ) return true;

    return false;
}

bool WorldGenerator::checkAdjacentCellsMatch(glm::ivec4 a, glm::ivec4 b) {
    glm::ivec4 offsets[] = {
    {-1,0,0,0},{1,0,0,0},
    {0,-1,0,0},{0,1,0,0},
    {0,0,-1,0},{0,0,1,0},
    {0,0,0,-1},{0,0,0,1}
    };

    for (int i = 0; i < 8; i++) {
        unsigned curr = getGridVal(a + offsets[i]);
        unsigned next = getGridVal(b + offsets[i]);

        curr = curr == 1 || curr == 3 ? 0 : curr;
        next = next == 1 || next == 3 ? 0 : next;

        curr = curr == 4 ? 2 : curr;
        next = next == 4 ? 2 : next;
        if (curr != next) return false;
    }

    return true;
}



int WorldGenerator::createMeshes() {

    //testGrid();
    
    std::vector<int> units;
    for (int i = 0; i < rooms.size(); i++) {

        glm::ivec4 min = rooms[i].minCorner;
        glm::ivec4 dim = rooms[i].dimensions;

        float h = (float)i / (float)rooms.size();
        float r = abs(h * 6.f - 3.f) - 1.f;
        float g = 2.f - abs(h * 6.f - 2.f);
        float b = 2.f - abs(h * 6.f - 4.f);
        glm::vec3 color = glm::clamp(glm::vec3(r, g, b), 0.f, 1.f);
    }



    for (int i = 0; i < gridRes; i++) {
        for (int j = 0; j < gridRes; j++) {
            for (int k = 0; k < gridRes; k++) {
                for (int l = 0; l < gridRes; l++) {
                    int currCell = worldGrid[i][j][k][l];
                    int currUnit;
                    if (currCell == 1) {
                        units.push_back(scene->Tesseract(glm::vec3(1.f, 0.f, 0.f)));
                    } else if (currCell == 2) {
                        units.push_back(scene->Tesseract(glm::vec3(0.f, 1.f, 0.f)));
                    }
                    else if (currCell == 3) {
                        units.push_back(scene->Tesseract(glm::vec3(1.f, 1.f, 0.f)));
                    }
                    else if (currCell == 4) {
                        units.push_back(scene->Tesseract(glm::vec3(0.5f, 0.f, 1.f)));
                    }
                    else {
                        continue;
                    }

                    glm::vec4 pos((float)i, (float)j, (float)k, (float)l);
                    pos -= glm::vec4(float(gridRes) / 2.f);
                    pos += glm::vec4(0.5f);
                    scene->translate(units[units.size() - 1], pos);
                }
            }
        }
    }

    int map = scene->groupEntities(units);
    scene->scale(map, glm::vec4(gridScale));
    scene->translate(map, glm::vec4((float)gridRes * gridScale, 0.f, 0.f, 0.f));
    

    std::vector<int> meshData;

    //add walls
        //for i = 0 - 8
        //constant component = i / 2
        //pos wall = i % 2 == 1
        //use those to determine starting corner (minCorner for neg walls, minCorner + const component size for pos wall)
        //use to determine ending corner (starting corner plus size of non const components)
        //keep track of a currMin and doorPos, starts at starting corner
        // 
        //check
            //first in one direction: are there any doorway cells here
            //then in the other direction: are there any doors in this 2d slice
        //if check returns no door
        //repeat check until check returns a door, set doorPos to this cell position
        //block of no doors from currMin to one less than doorPos in third dimension
        //block of no doors from currMin(with third dimension set to doorPos) to one less than doorPos in second dimension
        //block of no doors from currMin(with second and third dimension set to doorPos) to one less than doorPos in first dimension
        //note that for the above blocks, must also incorporate the offset of the door in the block in each direction based on doorDimensions
            
    //after one iteration of the check loop (one door found), set currMin to doorPos plus one in the first dimension and repeat
    //eventually instead of hitting a door cell, the loop will be terminated by hitting the ending corner of the wall. simply repeat the process 

    //you actually need to first go in the first dimension direction, until you are at the start of the next first dimensional segment
    //then in the second dimension direction until you are at the start of the next second dimensional segment
    //then you can check 2d slices until you hit the door pos, and go back down to the door

    //so final order is
        //finish row(1D)
        //finish grid(2D)
        //fill space(3D)
        //fill remaining grid(2D)
        //fill remaining row(1D)

    //the thing i didnt talk about here is translates
        //everything is initially spawned at the origin in world space
        //high level the steps are
            //translate so minCorner is at origin instead of center
            //translate to room from origin to match minCorner of cube with minCorner of room
            //then translate from minCorner of room to whatever grid cell corner it needs to align with

    //loop through rooms
    for (int i = 0; i < rooms.size(); i++) {

        //global grid coordinates
        glm::ivec4 minCorner = rooms[i].minCorner;
        glm::ivec4 dim = rooms[i].dimensions;
        std::vector<int> sections;
        glm::vec3 colors[8] = { {1.f, 0.f, 0.f}, {0.f, 1.f, 1.f}, {0.f, 1.f, 0.f}, {1.f, 0.f, 1.f}, {0.f, 0.f, 1.f}, {1.f, 1.f, 0.f}, {1.f, 1.f, 1.f}, {0.2f, 0.2f, 0.2f} };
        for (int j = 0; j < 8; j++) {
            colors[j] = { 1.f, 1.f, 0.f };
        }
        int numDoors = 0;

        //loop through walls
        for (int j = 0; j < 8; j++) {
            glm::vec3 color = colors[j];
            //-x, +x, -y, +y, -z, +z, -w, +w
            int side = j / 2;
            bool posSide = j % 2 == 1;

            glm::ivec4 startCorner = minCorner;
            if (posSide) {
                startCorner[side] += dim[side] - 1;
            }
            glm::ivec4 endCorner = startCorner + dim - glm::ivec4(1);
            endCorner[side] = startCorner[side];

            //side = x, a = y, b = z, c = w
            //side = y, a = z, b = w, c = x
            //side = z, a = w, b = x, c = y
            //side = w, a = x, b = y, c = z
            int a = (side + 1) % 4; //third dimension
            int b = (side + 2) % 4; //second dimension
            int c = (side + 3) % 4; //first dimension

            int flipped = j % 2 == 0 ? j + 1 : j - 1;

            float length;
            int wallSection;
                

            //local coordinates
            glm::ivec4 currMin(0);

            //loop through wall cells
            for (int k = 0; k < dim[a]; k++) {
                for (int l = 0; l < dim[b]; l++) {
                    for (int m = 0; m < dim[c]; m++) {
                        //local coordinates
                        glm::ivec4 offset(0);
                        offset[a] = k;  //third dimension
                        offset[b] = l;  //second dimension
                        offset[c] = m;  //first dimension
                        glm::ivec4 currCell = startCorner + offset;

                        //if current cell is a door
                        if (currMin != offset && getGridVal(currCell) == 4 && contains(doorways[currCell], j)) {
                            bool doorReached = false;
                            numDoors++;
                            //fill remaining row
                            if (currMin[c] != 0) {

                                length = (float)(dim[c] - currMin[c]);
                                if (currMin[a] == offset[a] && currMin[b] == offset[b]) {
                                    length = (float)(offset[c] - currMin[c] - 1);
                                    doorReached = true;
                                }

                                if (length > 0.f) {
                                    wallSection = getOrientedCube(flipped, color);
                                    glm::vec4 scalar(1.f);
                                    scalar[c] = length;
                                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);
          

                                    scene->scale(wallSection, scalar);
                                    scene->translate(wallSection, translate);
                                    sections.push_back(wallSection);
                                }

                                //increment currMin to next cell to be rendered
                                if (doorReached) {
                                    currMin[c] = offset[c] + 1;
                                    if (currMin[c] >= dim[c]) {
                                        currMin[b]++;
                                    }
                                }
                                else {
                                    currMin[c] = 0;
                                    currMin[b]++;
                                }

                                if (currMin[b] >= dim[b]) {
                                    currMin[b] = 0;
                                    currMin[a]++;
                                }
                            }

                            //fill remaining grid
                            if (!doorReached && currMin[b] != 0) {

                                length = (float)(dim[b] - currMin[b]);
                                if (currMin[a] == offset[a]) {
                                    length = (float)(offset[b] - currMin[b] - 1);
                                    doorReached = true;
                                }

                                if (length > 0.f) {
                                    wallSection = getOrientedCube(flipped, color);
                                    glm::vec4 scalar(1.f);
                                    scalar[c] = dim[c];
                                    scalar[b] = length;

                                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                                    scene->scale(wallSection, scalar);
                                    scene->translate(wallSection, translate);
                                    sections.push_back(wallSection);
                                }

                                //increment currMin to next cell
                                if (doorReached) {
                                    currMin[c] = 0;
                                    currMin[b] = offset[b];
                                    if (currMin[b] >= dim[b]) {
                                        currMin[b] = 0;
                                        currMin[a]++;
                                    }
                                }
                                else {
                                    currMin[b] = 0;
                                    currMin[a]++;
                                }
                            }

                            //fill grids between doors
                            int numGrids = offset[a] - currMin[a];
                            if (!doorReached && numGrids > 0) {
                                length = (float)numGrids;
                                wallSection = getOrientedCube(flipped, color);
                                glm::vec4 scalar(1.f);
                                scalar[c] = dim[c];
                                scalar[b] = dim[b];
                                scalar[a] = length;

                                glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                                scene->scale(wallSection, scalar);
                                scene->translate(wallSection, translate);
                                sections.push_back(wallSection);

                                currMin[a] += numGrids;
                            }

                            //fill remaining 2D section up to door
                            if (!doorReached && offset[b] != 0) {
                                length = (float)(offset[b] - currMin[b]);

                                if (length > 0.f) {
                                    wallSection = getOrientedCube(flipped, color);
                                    glm::vec4 scalar(1.f);
                                    scalar[c] = dim[c];
                                    scalar[b] = length;

                                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                                    scene->scale(wallSection, scalar);
                                    scene->translate(wallSection, translate);
                                    sections.push_back(wallSection);

                                    currMin[b] = offset[b];
                                }
                            } 

                            //fill remaining 1D section up to door
                            if (offset[c] != 0) {
                                length = (float)(offset[c] - currMin[c]);
                                if (length > 0.f) {
                                    wallSection = getOrientedCube(flipped, color);
                                    glm::vec4 scalar(1.f);
                                    scalar[c] = length;

                                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                                    scene->scale(wallSection, scalar);
                                    scene->translate(wallSection, translate);
                                    sections.push_back(wallSection);

                                    currMin[c] = offset[c] + 1;
                                    if (currMin[c] >= dim[c]) {
                                        currMin[c] = 0;
                                        currMin[b]++;
                                        if (currMin[b] >= dim[b]) {
                                            currMin[b] = 0;
                                            currMin[a]++;
                                        }
                                    }
                                }
                            }
                        } else {
                            if (currMin == offset && getGridVal(currCell) == 4) {
                                currMin[c]++;
                                if (currMin[c] >= dim[c])
                                {
                                    currMin[c] = 0;
                                    currMin[b]++;
                                    if (currMin[b] >= dim[b]) {
                                        currMin[b] = 0;
                                        currMin[a]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //after checking last cell
            if (currMin[a] < dim[a]) {
                if (currMin[c] != 0) {
                    length = (float)(dim[c] - currMin[c]);
                    wallSection = getOrientedCube(flipped, color);
                    glm::vec4 scalar(1.f);
                    scalar[c] = length;

                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                    scene->scale(wallSection, scalar);
                    scene->translate(wallSection, translate);
                    sections.push_back(wallSection);

                    //increment currMin to next cell to be rendered
                    currMin[c] = 0;
                    currMin[b]++;
                    if (currMin[b] >= dim[b]) {
                        currMin[b] = 0;
                        currMin[a]++;
                    }
                }

                //fill remaining grid
                if (currMin[b] != 0) {
                    length = (float)(dim[b] - currMin[b]);
                    if (length > 0.f) {
                        wallSection = getOrientedCube(flipped, color);
                        glm::vec4 scalar(1.f);
                        scalar[c] = dim[c];
                        scalar[b] = length;

                        glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);
                        scene->scale(wallSection, scalar);
                        scene->translate(wallSection, translate);
                        sections.push_back(wallSection);
                    }

                    //increment currMin to next cell
                    currMin[b] = 0;
                    currMin[a]++;
                }

                //fill grids between doors
                int numGrids = dim[a] - currMin[a];
                if (numGrids > 0) {
                    length = (float)numGrids;
                    wallSection = getOrientedCube(flipped, color);
                    glm::vec4 scalar(1.f);
                    scalar[c] = dim[c];
                    scalar[b] = dim[b];
                    scalar[a] = length;

                    glm::vec4 translate = getTranslate(scalar, startCorner, currMin, posSide, side);

                    scene->scale(wallSection, scalar);
                    scene->translate(wallSection, translate);
                    sections.push_back(wallSection);

                    currMin[a] += numGrids;
                }
            }
        }

        int room = scene->groupEntities(sections);
        glm::vec4 scalar = glm::vec4(gridScale);
        scene->scale(room, scalar);
        meshData.push_back(room);
    }

    
    
    
    //add straight corridors
    //accumulate list of all corridor paths when generating corridor cells
    //for each path go through each position, and if the position neighborhood matches the previous position neighborhood, append that position
    //store appended positions in a hallway struct
    //if a position neighborhood does not match, end current corridor, skip current position and restart

    std::vector<int> corridorMeshData;
    for (int i = 0; i < corridors.size(); i++) {
        
        glm::ivec4 start = corridors[i].start;
        glm::ivec4 end = corridors[i].end;
        int dir = corridors[i].direction;

        int numDifferences = 0;

        for (int j = 0; j < 4; j++) {
            if (start[j] != end[j]) {
                numDifferences++;
            }
        }
        if (numDifferences > 1) throw std::runtime_error("Invalid corridor");

        int corr = corridor( (float)(abs(end[dir] - start[dir]))  + 1, dir);
        corridorMeshData.push_back(corr);

        glm::vec4 translate(-(float)gridRes / 2.f + 0.5f);
        translate += glm::vec4(start + end) / 2.f;
        scene->translate(corr, translate);
    }

    //add corridor corners/intersections

    for (int i = 0; i < corridorCorners.size(); i++) {

        int corner = corridorCorner(corridorCorners[i].walledDirections);
        if (corner == -1) continue;
        corridorMeshData.push_back(corner);

        glm::vec4 translation(-(float)gridRes / 2.f + 0.5f);
        translation += glm::vec4(corridorCorners[i].position);
        scene->translate(corner, translation);
    }

    std::cout << "Line 783" << std::endl;
    int corridorSection = scene->groupEntities(corridorMeshData);
    scene->scale(corridorSection, glm::vec4(gridScale));

    return 0;
    return scene->groupEntities(meshData);
}



int getManhattanDistance(glm::ivec4 a, glm::ivec4 b) {
    glm::vec4 tA(a);
    glm::vec4 tB(b);
    glm::vec4 diff = tA - tB;
    for (int i = 0; i < 4; i++) { diff[i] = abs(diff[i]); }
    glm::ivec4 uDiff(diff);
    return uDiff.x + uDiff.y + uDiff.z + uDiff.w;
}

std::vector<glm::ivec4> WorldGenerator::aStar(int roomA, int roomB) {
    glm::ivec4 start = rooms[roomA].minCorner + rooms[roomA].dimensions / 2;
    glm::ivec4 goal = rooms[roomB].minCorner + rooms[roomB].dimensions / 2;

    std::vector<std::vector<int>> weights = {
        {10, 9999, 1, 10, 10},
        {10, 10, 1, 9999, 9999},
        {10, 9999, 1, 10, 10},
        {9999, 9999, 9999, 10, 10},
        {10, 10, 1, 9999, 9999}
    };


    glm::ivec4 offsets[] = {
    {1,0,0,0},{-1,0,0,0},
    {0,1,0,0},{0,-1,0,0},
    {0,0,1,0},{0,0,-1,0},
    {0,0,0,1},{0,0,0,-1}
    };

    
    Fringe fringe;
    fringe.push(0, getManhattanDistance(start, goal), start, start);

    int currIter = 0;

    int maxIter = gridRes * gridRes * gridRes * gridRes;

    while (!fringe.empty()) {
        currIter++;
        if (currIter > maxIter) {
            throw std::runtime_error("infinite loop");
        }
        int cost;
        glm::ivec4 prev;
        glm::ivec4 pos;
        bool valid = fringe.pop(cost, pos, prev);

        if (valid) {
            if (pos == goal) break;
            for (int i = 0; i < 8; i++) {
                glm::ivec4 newPos = glm::ivec4(glm::ivec4(pos) + offsets[i]);
                if (newPos.x >= gridRes || newPos.y >= gridRes || newPos.z >= gridRes || newPos.w >= gridRes) continue;
                if (newPos.x < 0 || newPos.y < 0 || newPos.z < 0 || newPos.w < 0) continue;
                
                int weight = weights[getGridVal(pos)][getGridVal(newPos)];
                int newG = cost + weight;
                fringe.push(newG, getManhattanDistance(newPos, goal), newPos, pos);
            }
        }
    }

    std::vector<glm::ivec4> path = fringe.getPath(start, goal);
    return path;
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
                glm::ivec4 centerA = rooms[i].minCorner + rooms[i].dimensions / 2;
                glm::ivec4 centerB = rooms[j].minCorner + rooms[j].dimensions / 2;
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



int WorldGenerator::getOrientedCube(int direction, glm::vec3 color) {
    int cube = scene->Cube(color);

    int planeID = 0;
    float sign = 1.f;

    //direction = direction % 2 == 0 ? direction + 1 : direction - 1;
    
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

int WorldGenerator::wallWithDoor(glm::vec4 dim, glm::vec3 doorPosition, int direction) {

    int minIndex = 0;
    for (int i = 1; i < 4; i++) {
        if (dim[i] < dim[minIndex]) {
            minIndex = i;
        }
    }

    int aIndex = (minIndex + 1) % 4;
    int bIndex = (minIndex + 2) % 4;
    int cIndex = (minIndex + 3) % 4;

    glm::vec4 doorDim = doorDimensions * gridScale;
    doorDim[3] = dim[minIndex];
    glm::vec4 doorPos(0.f);
    doorPos[aIndex] = doorPosition.x;
    doorPos[bIndex] = doorPosition.y;
    doorPos[1] = doorDim[1] / 2.f - dim[1] / 2.f;

    int aNeg = getOrientedCube(direction);
    int aPos = getOrientedCube(direction);
    int yPos = getOrientedCube(direction);
    int bNeg = getOrientedCube(direction);
    int bPos = getOrientedCube(direction);



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
    }


    return scene->groupEntities(sections);
}

int WorldGenerator::corridorCorner(std::vector<int> walledDirections){
    if (walledDirections.size() == 0) return -1;
    std::vector<int> walls;

    for (int i = 0; i < walledDirections.size(); i++) {
        int dir = walledDirections[i] % 2 == 0 ? walledDirections[i] + 1 : walledDirections[i] - 1;
        walls.push_back(getOrientedCube(dir, glm::vec3(0.f, 1.f, 1.f)));
    }

    for (int i = 0; i < walledDirections.size(); i++) {
        int wall = walledDirections[i];
        glm::vec4 translate(0.f);
        translate[wall / 2] = (float)(wall % 2 - 0.5f);
        
        scene->translate(walls[i], translate);
    }


    int corner = scene->groupEntities(walls);
    scene->scale(corner, doorDimensions);

    std::vector<int> fullCorner = { corner };
    for (int i = 0; i < 8; i++) {
        if (!contains(walledDirections, i)) {
            float length = (1.f - doorDimensions[i / 2]) / 2.f;
            int connection = corridor(length, i / 2);
            if (connection == -1) continue;
            glm::vec4 translate(0.f);
            translate[i / 2] = ((float)(i % 2) * 2.f - 1.f) * (0.5f - length / 2.f);
            scene->translate(connection, translate);
            fullCorner.push_back(connection);
        }
    }
    return scene->groupEntities(fullCorner);
}

int WorldGenerator::corridor(float length, int directionDim) {
    if (length == 0) return -1;
    glm::vec3 color(0.f);
    if (directionDim == 0) color = glm::vec3(1.f, 0.f, 0.f);
    if (directionDim == 1) color = glm::vec3(0.f, 1.f, 0.f);
    if (directionDim == 2) color = glm::vec3(0.f, 0.f, 1.f);
    if (directionDim == 3) color = glm::vec3(1.f, 1.f, 1.f);
    std::vector<int> walls;
    for (int i = 0; i < 8; i++) {
        if (i / 2 != directionDim) {
            walls.push_back(getOrientedCube(i, color));
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
    int hall = scene->groupEntities(walls);
    scene->scale(hall, dim);
    return hall;
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