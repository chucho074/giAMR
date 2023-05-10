/**
 * @file    giAMR.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   A basic description of the what do the doc.
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
    String tmpPath = Configs::s_outPath.string()
                       + inData.filename().string();
    m_savedData.m_outputDir = tmpPath;
    m_renderWindow = true;
  }

  void 
  AMR::run() {
    //Add a way to verify if there's info to use
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
    json tmpFile;

    tmpFile["base_mesh"] = m_savedData.m_baseMesh.c_str();

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

    tmpFile["out_dir"] = m_savedData.m_outputDir.c_str();

    ofstream o(g_Configs().s_binPath.string()+"giAMR.json");
    o << std::setw(4) << tmpFile << ConsoleLine;
  }
  

  void 
  AMR::createCommands() {
    String tmpOut;

    tmpOut += "@echo off\n";
    tmpOut += "call \"" + g_Configs().s_anacondaPath.string() + "\" activate dmodel\n";
    tmpOut += "cd /d \"" + g_Configs().s_nvdiffmodPath.string()+ "\"\n";
    tmpOut += "cmd /c \"python giAMR.py --config" + g_Configs().s_nvdiffmodPath.string() +" & exit\"\"";

    ofstream fout(g_Configs().s_binPath.string());
    fout << tmpOut;
  }



  AMR& 
  g_AMR() {
    return AMR::instance();
  }
}