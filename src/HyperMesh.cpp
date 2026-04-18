#include "HyperMesh.h"
#include <glm/glm.hpp>
#include <iostream>
HyperMesh::HyperMesh(std::vector<Vertex> vData, std::vector<GLuint> triData, std::vector<GLuint> edges, std::vector<std::vector<GLuint>> faces){
    vertexData = vData;
    triangleData = triData;
    edgeData = edges;
    faceData = faces;
    numTriangleVertices = triangleData.size();
    numEdgeVertices = edgeData.size();
    createVertexObjects();
}

void HyperMesh::createVertexObjects(){
    //triangles vao
    glGenVertexArrays(1, &vaoTriangles);
    glBindVertexArray(vaoTriangles);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &iboTriangles);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboTriangles);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleData.size() * sizeof(GLuint), triangleData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    glDisableVertexAttribArray(0);

    //edges vao
    glGenVertexArrays(1, &vaoEdges);
    glBindVertexArray(vaoEdges);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &iboEdges);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboEdges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeData.size() * sizeof(GLuint), edgeData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    //deallocates
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

std::vector<glm::vec4> HyperMesh::getVertexData(){
    std::vector<glm::vec4> vertices;
    for(Vertex v : vertexData){
        vertices.push_back(v.position);
    }

    return vertices;
}


HyperMesh HyperMesh::Tesseract(){
    std::vector<Vertex> vertices;

    std::vector<glm::vec4> cellNormals = 
    {
        glm::vec4(-1.f, 0.f, 0.f, 0.f),         //left
        glm::vec4(1.f, 0.f, 0.f, 0.f),          //right
        glm::vec4(0.f, -1.f, 0.f, 0.f),         //bottom
        glm::vec4(0.f, 1.f, 0.f, 0.f),          //top

        glm::vec4(0.f, 0.f, -1.f, 0.f),         //front
        glm::vec4(0.f, 0.f, 1.f, 0.f),          //back
        glm::vec4(0.f, 0.f, 0.f, -1.f),         //in
        glm::vec4(0.f, 0.f, 0.f, 1.f),          //out
    }

    std::vector<std::vector<glm::vec4>> vertexNormals = 
    {
        {cellNormals[0], cellNormals[2], cellNormals[4], cellNormals[6]},
        {cellNormals[0], cellNormals[2], cellNormals[4], cellNormals[7]},
        {cellNormals[0], cellNormals[2], cellNormals[5], cellNormals[6]},
        {cellNormals[0], cellNormals[2], cellNormals[5], cellNormals[7]},

        {cellNormals[0], cellNormals[3], cellNormals[4], cellNormals[6]},
        {cellNormals[0], cellNormals[3], cellNormals[4], cellNormals[7]},
        {cellNormals[0], cellNormals[3], cellNormals[5], cellNormals[6]},
        {cellNormals[0], cellNormals[3], cellNormals[5], cellNormals[7]},

        {cellNormals[1], cellNormals[2], cellNormals[4], cellNormals[6]},
        {cellNormals[1], cellNormals[2], cellNormals[4], cellNormals[7]},
        {cellNormals[1], cellNormals[2], cellNormals[5], cellNormals[6]},
        {cellNormals[1], cellNormals[2], cellNormals[5], cellNormals[7]},

        {cellNormals[1], cellNormals[3], cellNormals[4], cellNormals[6]},
        {cellNormals[1], cellNormals[3], cellNormals[4], cellNormals[7]},
        {cellNormals[1], cellNormals[3], cellNormals[5], cellNormals[6]},
        {cellNormals[1], cellNormals[3], cellNormals[5], cellNormals[7]},
    }

    for(int i = 0; i < 4; i++){

        for(int k = 0; k < 2; k++){
            for(int l = 0; l < 2; l++){
                for(int m = 0; m < 2; m++){
                    for(int n = 0; n < 2; n++){
                        Vertex newVertex((float)k - 0.5f, (float)l - 0.5f, (float)m - 0.5f, (float)n - 0.5f);
                        int vertexNumber = 8 * k + 4 * l + 2 * m + n;
                        newVertex.normal = vertexNormals[vertexNumber][i];
                        vertices.push_back(newVertex);
                    }
                }
            }
        }

    }

    //DO NOT CHANGE ORDER
    std::vector<std::vector<int>> perCellVertexData = 
    {
        {1, 3, 5, 7, 0, 2, 4, 6},               //left
        {8, 10, 12, 14, 9, 11, 13, 15},         //right
        {9, 11, 8, 10, 17, 19, 16, 18},         //bottom
        {12, 14, 13, 15, 20, 22, 21, 23},       //top
        {17, 16, 21, 20, 25, 24, 29, 28},       //front
        {18, 19, 22, 23, 26, 27, 30, 31},       //back
        {48, 50, 52, 54, 56, 58, 60, 62},       //in
        {49, 51, 53, 55, 57, 59, 61, 63}        //out
    }

    //DO NOT CHANGE ORDER
    std::vector<int> perCellIndexData =     //tetrahedrons per cell
    {
        0, 1, 3, 4,
        0, 2, 3, 4,
        1, 3, 4, 5,
        3, 4, 5, 7,
        2, 3, 4, 6,
        3, 4, 6, 7
    }

    std::vector<uint16_t> indexData;

    for(int i = 0; i < perCellVertexData.size(); i++){
        for(int j = 0; j < perCellIndexData.size(); j++){
            indexData.push_back(perCellVertexData[i][perCellIndexData[j]]);
        }
    }



    
}

HyperMesh HyperMesh::CubicPyramid(){
    std::vector<Vertex> vertices;

    for(int i = 0;i < 2; i++){
        for(int j = 0; j < 2; j++){
            for(int k = 0; k < 2; k++){
                    Vertex newVertex((float)i - 0.5f, (float)j - 0.5f, (float)k - 0.5f, -0.5f);
                    vertices.push_back(newVertex);
            }
        }
    }

    Vertex newVertex(0.f, 0.f, 0.f, 0.5f);
    vertices.push_back(newVertex);

    //triangle indices

    std::vector<GLuint> triangleIndices = {
        //cube faces
        1, 7, 3, 1, 5, 7,   //front
        5, 6, 7, 5, 4, 6,   //right
        4, 2, 6, 4, 0, 2,   //back
        0, 3, 2, 0, 1, 3,   //left
        0, 5, 1, 0, 4, 5,   //bottom
        3, 6, 2, 3, 7, 6,    //top

        //pyramids
        3, 7, 8, 7, 5, 8, 5, 1, 8, 5, 1, 3,  //front
        7, 6, 8, 6, 4, 8, 4, 5, 8, 5, 7, 8, //right
        6, 2, 8, 2, 0, 8, 0, 4, 8, 4, 6, 8,  //back
        2, 3, 8, 3, 1, 8, 1, 0, 8, 0, 2, 8,  //left
        1, 5, 8, 5, 4, 8, 4, 0, 8, 0, 1, 8,  //bottom
        2, 6, 8, 6, 7, 8, 7, 3, 8, 3, 2, 8  //top
    };

    std::vector<GLuint> lineIndices
    {
        3, 7, 7, 5, 5, 1, 1, 3, //front - 0
        7, 6, 6, 4, 4, 5, 5, 7, //right - 4
        6, 2, 2, 0, 0, 4, 4, 6, //back - 8
        2, 3, 3, 1, 1, 0, 0, 2, //left - 12
        1, 5, 5, 4, 4, 0, 0, 1,  //bottom - 16
        2, 6, 6, 7, 7, 3, 3, 2, //top - 20

        0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 5, 8, 6, 8, 7, 8 //pyramids - 24
    };

    std::vector<std::vector<GLuint>> faceIndices{
        
        //cube
        {0, 1, 2, 3},       //front
        {4, 5, 6, 7},       //right
        {8, 9, 10, 11},     //back
        {12, 13, 14, 15},   //left
        {16, 17, 18, 19},   //bottom
        {20, 21, 22, 23},   //top

        //pyramids
        {0, 27, 31},        //front
        {1, 29, 31},
        {2, 25, 29},
        {3, 25, 27},

        {4, 30, 31},        //right
        {5, 28, 30},
        {6, 28, 29},
        {7, 29, 31},

        {7, 26, 30},        //back
        {8, 24, 26},
        {9, 24, 28},
        {10, 28, 30},

        {11, 26, 27},       //left
        {12, 25, 27},
        {13, 24, 25},
        {14, 24, 26},

        {15, 25, 29},       //bottom
        {16, 28, 29},
        {17, 24, 28},
        {18, 24, 25},

        {19, 26, 30},       //top
        {20, 30, 31},
        {21, 27, 31},
        {22, 26, 27}
    };

    HyperMesh m(vertices, triangleIndices, lineIndices, faceIndices);
    return m;
}

HyperMesh HyperMesh::Cube(){
    
    //position data

    float l = 0.5f;
    float w = 0.5f;
    float h = 0.5f;

    std::vector<Vertex> vertexData = {
        Vertex(w, h, -l, 0.f), //0 - right top back
        Vertex(w, h, l, 0.f), //1 - right top front
        Vertex(w, -h, -l, 0.f), //2 - right bottom back
        Vertex(w, -h, l, 0.f), //3 - right bottom front
        Vertex(-w, h, -l, 0.f), //4 - left top back 
        Vertex(-w, h, l, 0.f), //5 - left top front
        Vertex(-w, -h, -l, 0.f), //6 - left bottom back
        Vertex(-w, -h, l, 0.f), //7 - left bottom front
    };
    
    const std::vector<GLuint> indexData = {
        7, 1, 5, 7, 3, 1, //front
        11, 0, 9, 11, 2, 0, //right
        10, 4, 8, 10, 6, 4, //back
        14, 13, 12, 14, 15, 13, //left
        21, 16, 20, 21, 17, 16, //top
        22, 19, 23, 22, 18, 19 //down
    };

    HyperMesh m(vertexData, indexData, {}, {});
    return m;
}