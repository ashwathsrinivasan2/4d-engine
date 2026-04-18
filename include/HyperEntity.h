#ifndef HYPERENTITY_H
#define HYPERENTITY_H

#include <vector>
#include "Entity.h"
#include "HyperMesh.h"
#include "HyperTransform.h"
#include <iostream>

class HyperEntity{
    HyperMesh       meshData;
    HyperTransform  transform;

    //instance data
    std::vector<glm::mat4>  instanceScales;
    std::vector<glm::vec4>  instanceTranslates;
    GLuint                  instanceScaleVBO;
    GLuint                  instanceTranslateVBO;
    unsigned                numInstances                = 0;

    //edge data
    struct Edge{
        glm::vec4 minPoint; //minPoint has min w value
        glm::vec4 maxPoint;

        int minIndex;
        int maxIndex;

        std::vector<GLuint> faceIndices;

        Edge(glm::vec4 min, glm::vec4 max, int iMin, int iMax)
        : minPoint(min), maxPoint(max), minIndex(iMin), maxIndex(iMax){}
    };
    std::vector<Edge> edges;

    void quickSort(std::vector<Edge>&);
    void quickSortHelper(std::vector<Edge>&, int, int);

    //cross section functionality
    std::vector<Entity> crossSections;
    int                 crossSectionDetail = 100;
    int                 currCrossSection = crossSectionDetail / 2;
    float               maxW = 0.5f;
    float               minW = -0.5f;
    void                updateWRange();
    void                createCrossSection(float);
    bool                linearlyDependent(glm::vec4, glm::vec4, glm::vec4);
    bool                samePlane(glm::vec4 basisA, glm::vec4 basisB, int pointA, int pointB, int depth = 3);
    bool                nearlyEqual(glm::vec4 a, glm::vec4 b, float eps = 1e-6f);
    bool                sharesFace(Edge a, Edge b);

    //texture + color
    struct Texture{
        int textureWidth, textureHeight, numChannels;
        GLuint texObj;
        unsigned char* path = nullptr;
        bool active;
    };
    Texture tex;
    glm::vec3 color = glm::vec3(1.f);

    public:
    HyperEntity(HyperMesh mesh);
    glm::mat4 getModelMatrix(){return transform.getModelMatrix();}
    glm::vec4 getTranslate(){return transform.getTranslate();}

    //color
    void        setColor(glm::vec3 rgb){color = rgb;}
    glm::vec3   getColor(){return color;}

    //transform
    void rotate(float, int, int, int, int);
    void translate(glm::vec4);
    void scale(glm::vec4);
    void getRotor(float&, float&, float&, float&, float&, float&, float&);

    //instancing
    void    addInstance(glm::mat4, glm::vec4);
    int     getNumInstances();
    void    createInstanceVBO();

    //mesh interaction
    GLuint      getTriangleVao()                {return meshData.getTriangleVao();}
    GLuint      getEdgeVao()            {return meshData.getEdgeVao();}
    GLuint      getVbo()                {return meshData.getVbo();}
    int         getNumTriangleVertices()        {return meshData.getNumTriangleVertices();}
    int         getNumEdgeVertices()    {return  meshData.getNumEdgeVertices();}
    int         getNuVertices()          {return meshData.getNumVertices();}
    glm::vec4   getVertex(int index)    {return meshData.getVertexData()[index];}

    //cross sections
    void    generateCrossSections();
    Entity& getCrossSection(){return crossSections[currCrossSection];}
    void incrementCrossSection(){if(currCrossSection < crossSectionDetail) currCrossSection++; std::cout << currCrossSection << std::endl;}
    void decrementCrossSection(){if(currCrossSection > 0) currCrossSection--;std::cout << currCrossSection << std::endl;}

    void reset();
    void test(){transform.test();}

    //error checking
    void printInstances(){
        for(int i = 0; i < numInstances; i++){
            std::cout   << "T: "
                        << instanceTranslates[i].x << ", "
                        << instanceTranslates[i].y << ", "
                        << instanceTranslates[i].z << ", "
                        << instanceTranslates[i].w
                        << "    S: "
                        << instanceScales[i][0][0] << ", "
                        << instanceScales[i][1][1] << ", "
                        << instanceScales[i][2][2] << ", "
                        << instanceScales[i][3][3]
                        << std::endl;
        }       
    }
};

#endif