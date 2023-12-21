/**
 * @file    giImporter.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    20/12/2023
 * @brief   A basic implementation for import other types 
             of models diferent than obj.
 */
 
/**
 * @include
 */

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "giImporter.h"


namespace giAMRSDK {
  
  void
  processData(ModelInfo& inInfo, 
                   aiNode* node,
                   const aiScene* inScene) {
    for (uint32 i = 0; i < node->mNumMeshes; ++i) {
      aiMesh* mesh = inScene->mMeshes[node->mMeshes[i]];
      inInfo.totalVertices += mesh->mNumVertices;
      inInfo.totalFaces += mesh->mNumFaces;
      for (uint32 j = 0; j < mesh->mNumFaces; ++j) {
        aiFace face = mesh->mFaces[j];
        inInfo.totalIndex += face.mNumIndices;
      }
    }
    // then do the same for each of its children
    for (uint32 i = 0; i < node->mNumChildren; i++) {
      processData(inInfo, node->mChildren[i], inScene);
    }
  }

  void 
  Importer::readBasicModel(Path& inFile, ModelInfo& inInfo) {
    Assimp::Importer importer;

    importer.ReadFile(inFile.string(),
                      aiProcessPreset_TargetRealtime_MaxQuality |
                      aiProcess_TransformUVCoords |
                      aiProcess_ConvertToLeftHanded |
                      aiProcess_Triangulate);

    const aiScene* tmpScene = importer.GetOrphanedScene();

    if (!tmpScene
      || tmpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
      || !tmpScene->mRootNode) {
      __debugbreak();
      //g_logger().SetError(ERROR_TYPE::kModelLoading, "Failed to load a model");
    }

    inInfo.totalMeshes = tmpScene->mRootNode->mNumMeshes;
    inInfo.totalAnimations = tmpScene->mNumAnimations;
    inInfo.totalMaterials = tmpScene->mNumMaterials;

    processData(inInfo, tmpScene->mRootNode, tmpScene);

    inInfo.totalTriangles = inInfo.totalIndex / 3;
  }


}