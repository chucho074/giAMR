/**
 * @file    giAMR.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   The interface for the usage of Auto Model Redux.
 */
 
/**
 * @include
 */
#include "giAMR.h"
#include "giConfigs.h"

namespace giAMRSDK {

  void 
  AMR::setRefMesh(Path inData) {
    m_savedData.m_refMesh = inData;
    String tmpPath = Configs::s_generatedPath.string()
                       + inData.filename().string();
    m_savedData.m_outputDir = tmpPath;
  }

  void 
  AMR::run() {
    //Add a way to verify if there's info to use
    int32 tmpIter = 1;
    bool isCreated = false;
    while (!isCreated) {
      if(exists(m_savedData.m_outputDir.string() + "/(" + toString(tmpIter) + ")")) {
        tmpIter++;
      }
      else {
        m_savedData.m_outputDir = m_savedData.m_outputDir.string() + "/(" + toString(tmpIter) + ")";
        create_directories(m_savedData.m_outputDir);
        break;
      }
    }
    createJSON();
    createCommands();
    system("cmd /c start giAMR.bat");
  }

  void 
  AMR::reset() {
    m_processImg = 0;
  }

  void
  AMR::createJSON() {
    auto & configs = g_Configs();
    json tmpFile;

    //tmpFile["base_mesh"] = m_savedData.m_baseMesh.c_str();
    tmpFile["base_mesh"] = configs.s_generatedPath.string() + "sphere.obj";

    tmpFile["ref_mesh"] = m_savedData.m_refMesh.c_str();

    tmpFile["camera_eye"] = { 2.5, 0.0, 2.5 };

    tmpFile["camera_up"] = { 0.0, 1.0, 0.0 };

    tmpFile["save_interval"] = m_savedData.m_saveInterval;

    tmpFile["random_textures"] = m_savedData.m_randomTextures;

    tmpFile["train_res"] = m_savedData.m_trainResolution;

    tmpFile["barch"] = m_savedData.m_batch;

    tmpFile["learning_rate"] = m_savedData.m_learningRate;

    tmpFile["skip_train"] = { "kd","ks" };

    tmpFile["iter"] = m_savedData.m_iterations;

    tmpFile["out_dir"] = m_savedData.m_outputDir.make_preferred().c_str();

    ofstream o(g_Configs().s_binPath.string() + "giAMR.json");
    o << std::setw(4) << tmpFile << ConsoleLine;
  }
  

  void 
  AMR::createCommands() {
    String tmpOut;

    tmpOut += "@echo off\n";
    tmpOut += "call \"" + g_Configs().s_anacondaPath.string() + "/activate.bat\" activate dmodel\n";
    tmpOut += "cd /d \"" + g_Configs().s_nvdiffmodPath.string()+ "\"\n";
    tmpOut += "cmd /c \"python giAMR.py --config " + g_Configs().s_binPath.string() +"giAMR.json\"\"";

    ofstream fout(g_Configs().s_binPath.string()+"giAMR.bat");
    fout << tmpOut;
  }



  AMR& 
  g_AMR() {
    return AMR::instance();
  }
}