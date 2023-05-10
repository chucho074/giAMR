/**
 * @file    giModel.h
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
#include "giMesh.h"

namespace giAMRSDK {

  struct ModelInfo {
    uint32 totalTriangles = 0;
    uint32 totalVertices = 0;
    uint32 totalMeshes = 0;
    uint32 totalMaterials = 0;
    uint32 totalAnimations = 0;
    uint32 totalFaces = 0;
    uint32 totalIndex = 0;
  };
  
  class Model final
  {
   public:
  	Model() = default;
  	~Model() = default;

    //Meshes.
    Vector<SharedPtr<Mesh>> m_meshes;

    Path m_path;
  };
}