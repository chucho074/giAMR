/**
 * @file    giAMR.h
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
#include "giModel.h"
#include "giModule.h"

namespace giAMRSDK {
  struct DescAMR {
    int32 m_saveInterval = 512;

    bool m_randomTextures = false;

    Vector2 m_textureSize = { 2048, 2048 };

    int32 m_trainResolution = 512;

    int32 m_batch = 8;

    float m_learningRate = 0.01f;

    Vector<String> m_skipTrain;

    int32 m_iterations = 5000;

    Path m_outputDir;

    Path m_refMesh;

    Path m_baseMesh = "G:/Dev/giEngine/bin/Resources/Models/Generated/sphere.obj";
  };

  /**
   * @class      AMR. (Auto Model Redux)
   * @brief      This class is for the utilities to use Appearance-Driven
                  Automatic 3D Model Simplification, develop by NVIDIA, this is for
                  create an LOD (Level Of Detail) for the projects in giEngine.
   */
  class AMR : public Module<AMR>
  {
   public:
    AMR() = default;
    ~AMR() = default;
   
    /**
     * @brief    Set the reference mesh to work.
     * @param    inData        The path of the reference mesh.
     */
    void
    setRefMesh(Path inData);
    
    /**
     * @brief    Runs the tool. 
     */
    void
    run();
    
    /**
     * @brief    Reset's the information for use it again. 
     */
    void
    reset();

   private:

    /**
     * @brief    Creates the JSON file for the confings of ADA3DMS.
     */
    void
    createJSON();
    
    /**
     * @brief    Create the .bat for run ADA3DMS.
     */
    void
    createCommands();


   public:
    bool m_renderWindow = false;

    bool m_processWindow = false;

    int32 m_processImg = 0;

    int32 m_showingImg = 1;

    DescAMR m_savedData;

    ModelInfo m_refInfo;
    
    ModelInfo m_outInfo;

    uint32 minimunTriang = 512;
    uint32 halfTriang = 512;
    uint32 partialTriang = 512;

    int32 finalTriang = 0;

    //giAMR process image
    Vector<Texture> m_AMRprocess;

  };

  AMR&
  g_AMR();
}