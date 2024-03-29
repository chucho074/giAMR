/**
 * @file    giExporter.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   An exporter for the data used.
 */
 
/**
 * @include
 */

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/Exporter.hpp>      // C++ exporter interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "giExporter.h"


namespace giAMRSDK {
  
  void 
  Exporter::ExportObj(Path inPath, SharedPtr<Model> inModel) {
     
    String tmpOut;
    String tmpOutMtl;

    //Material data
    tmpOut += ("mtllib " + inPath.stem().string() + ".mtl\n");
    tmpOut += ("g default\n");

    int32 matID = 0;

    for(auto mesh : inModel->m_meshes) {
      //Vertex data
      for(auto vertex : mesh->m_vertexVector) {
        tmpOut += "v "+ toString(vertex.Pos.x)+ " "+ toString(vertex.Pos.y)+ " "+ toString(vertex.Pos.z)+ "\n";
      }

      //Vertex texture Coords
      for (auto vertex : mesh->m_vertexVector) {
        tmpOut += "vt "+ toString(vertex.Tex.x)+ " "+ toString(vertex.Tex.y)+ "\n";
      }

      //Vertex Normals data
      for (auto vertex : mesh->m_vertexVector) {
        tmpOut += "vn "+ toString(vertex.Pos.x)+ " "+ toString(vertex.Pos.y)+ " "+ toString(vertex.Pos.z)+ "\n";
      }

      //Smooth shading
      tmpOut += "s off\n";

      tmpOut += "g "+ inPath.stem().string()+ "_subd:polyToSubd1" + "\n";

      //Usemtl
      tmpOut += "usemtl mat"+ toString(matID) + "\n";
      matID++;


      //Faces data

      String faces;
      for (int32 i = 0; i < mesh->m_facesList.size(); i += 3) {
        int32 v1 = mesh->m_facesList[i] + 1;
        int32 v2 = mesh->m_facesList[i + 1] + 1;
        int32 v3 = mesh->m_facesList[i + 2] + 1;
        faces += "f " + toString(v1) + '/' + toString(v1) + '/' + toString(v1) + ' ' + toString(v2) + '/' + toString(v2) + '/' + toString(v2) + ' ' + toString(v3) + '/' + toString(v3) + '/' + toString(v3) + '\n';
      }
      tmpOut += faces;
    }
   
    
    //Write files
    ofstream fout(inPath.string());
    fout << tmpOut;

    ExportMtl(inPath);
  }

  void 
  Exporter::ExportMtl(Path inPath) {
    String tmpOutMtl;

    //Material data
    tmpOutMtl += "newmtl mat0\n";
    tmpOutMtl += "illum 4\n";

    tmpOutMtl += ("Kd " + toString(1.00) + " " + toString(1.00) + " " + toString(1.00) + "\n");
    tmpOutMtl += ("Ks " + toString(0.90) + " " + toString(0.90) + " " + toString(0.90) + "\n");
    tmpOutMtl += ("Ka " + toString(1.00) + " " + toString(1.00) + " " + toString(1.00) + "\n");
    tmpOutMtl += ("Tf " + toString(1.00) + " " + toString(1.00) + " " + toString(1.00) + "\n");
    tmpOutMtl += ("Ni " + toString(1.00) + "\n");


    Path tmpMtlPath = inPath;
    tmpMtlPath.replace_extension(".mtl");

    ofstream foutMtl(tmpMtlPath.string());
    foutMtl << tmpOutMtl;
  }

  Path
  Exporter::ExportAsObj(Path inPath, String inFileType) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(inPath.string(), 
                                             aiProcess_Triangulate   
                                             | aiProcess_FlipUVs);

    //Export the model.
    Assimp::Exporter exporter;
    inPath.replace_extension(inFileType);
    exporter.Export(scene, inFileType, inPath.string());
    return inPath;
  }
}