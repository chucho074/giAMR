/**
 * @file    giConfigs.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   The basic configurations for the project.
 */
 
/**
 * @include
 */
#pragma once
#include "giPrerequisitesAMR.h"
#include "giModule.h"

using namespace giAMRSDK;

class Configs : public Module<Configs>
{
 public:
  Configs() = default;
  ~Configs() = default;


  static Path s_projectPath;
  static Path s_anacondaPath;
  static Path s_nvdiffmodPath;
  static Path s_binPath;


  static Path s_generatedPath;
  
};

Configs&
g_Configs();