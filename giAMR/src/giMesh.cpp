/**
 * @file    giMesh.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   A basic description of the what do the doc.
 */
 
/**
 * @include
 */
#include "giMesh.h"

namespace giAMRSDK {
  Mesh::Mesh(Vector<SimpleVertex> inVertex, 
             Vector<uint32> inIndex) {
    m_vertexVector = inVertex;
    m_facesList = inIndex;

  }
  
  Mesh::~Mesh() {
    destroy();
  }

  void 
  Mesh::destroy() {
    m_facesList.clear();
    m_vertexVector.clear();
  }

  
}