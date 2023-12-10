/**
 * @file    giMesh.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    08/05/2023
 * @brief   A basic description of the what do the doc.
 */
 
/**
 * @include
 */
#pragma once
#include "giPrerequisitesAMR.h"
#include "giTexture.h"
#include "giVector2.h"
#include "giVector3.h"

namespace giAMRSDK {
  /**
   * @struct    SimpleVertex.
   * @brief     Basic struc for my vertex.
   */
  struct SimpleVertex {
    Vector3 Pos;
    Vector2 Tex;  //UV
    Vector3 Nor;
    Vector3 Tang;
    Vector3 BiNor;
  };

  class Mesh
  {
   public:
     /**
      * @brief    .
      * @param    inVertex      .
      * @param    inIndex       .
      * @param    inTextures    .
      */
    Mesh(Vector<SimpleVertex> inVertex,
         Vector<uint32> inIndex);

  	~Mesh();

    void 
    destroy();

    //A vertex data Vector
    Vector<SimpleVertex> m_vertexVector;

    //The list of Faces / Index data
    Vector<uint32> m_facesList;

    //Name of the mesh.
    String m_name;
  };
}