#ifndef HYPERMESH_H
#define HYPERMESH_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class HyperMesh{

    struct Vertex{
        glm::vec4 position;
        glm::vec4 normal;

        Vertex(float x, float y, float z, float w)
        : position(x,y,z,w) {}
    };

    //meshes should be made up of lists of tetrahedra instead of triangles
    //there is only one way to draw a tetrahedron given 4 points in 3d space (no front or back)
    //there is a "front and back" in 4D though. 
    //but we dont need to worry about face culling until the 3D stage

    //in 3D
        //each triangle has a forward/normal vector
        //if angle between camera forward vector and triangle forward vector is <90, render the triangle
        //for rendered triangles 
            //model matrix is trivial
            //view matrix 

    //in 4D
        //each tetrahedron has a forward/normal 4D vector
        //if angle between camera forward vector and tetrahedron forward vector is <90, render the tetrahedron
        //for rendered tetrahedrons
            //model matrix for entity scale and rotate, model vector for translate

            //view matrix for camera rotate, view vector for translate
            /*
                R - normalized x-dir camera vector
                U - normalized y-dir camera vector
                F - normalized z-dir camera vector
                A - normalized w-dir camera vector
                C - camera position


                            VIEW MATRIX                     VTX POS             CAM POS
                    {   {Rx     Ux     Fx     Ax}                  }
                    {   {Ry     Uy     Fy     Ay}      *       P   }      -       C
                    {   {Rz     Uz     Fz     Az}                  }
                    {   {Rw     Uw     Fw     Aw}                  }
            */

            //proj matrix for 4D -> 3D
            /*
                theta - vertical FOV of camera
                AR1 - length(x) / height(y)
                AR2 - width(z) / height(y)

                {   {1 / ( tan ( theta / 2 ) * AR1)                 0                             0                 0}                  }
                {   {              0                    1 / ( tan ( theta / 2 ) )                 0                 0}      *       P   }
                {   {              0                                0              1 / ( tan ( theta / 2 ) * AR2)   0}                  }       /       P.w
                {   {              0                                0                             0                 1}                  }
            */


           //[ [ [ [4D_PROJ_MATRIX] * [ [ 4D_VIEW_MATRIX ] * [ [ P * [4D_MODEL_MATRIX] ] + [4D_MODEL_TRANSLATE] ] + [4D_VIEW_TRANSLATE] ] ] / P.w ].xyz
           //need some sort of depth buffering here to check for object overlap
           //also need a way of saying which way a 4D object is pointed 
           //[ [ 3D_PROJ_MATRIX ] * [ 3D_VIEW_MATRIX ] * [ 3D_MODEL_MATRIX ] * P_3D ] / P_3D.z


    //Cells:
    /*
        out: 1, 3, 5, 7, 9, 11, 13, 15
        front: 1, 0, 5, 4, 9, 8, 13, 12
        left: 1, 3, 5, 7, 0, 2, 4, 6
        right: 8, 10, 11, 14, 9, 11, 13, 15
        back: 2, 3, 6, 7, 10, 11, 14, 15
        top: 4, 6, 5, 7, 12, 14, 13, 15
        bottom: 1, 3, 0, 2, 9, 11, 8, 10
        in: 0, 2, 4, 6, 8, 10, 12, 14
    */
    std::vector<Vertex> vertexData;

    std::vector<uint16_t> indexData;
};

#endif