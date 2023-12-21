/**
 * @file    giConfigs.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   The configurations for the app.
 */
 
/**
 * @include
 */
#include "giConfigs.h"


Path Configs::s_projectPath = ("");
Path Configs::s_anacondaPath = ("");
Path Configs::s_nvdiffmodPath = ("");
Path Configs::s_binPath = ("");
Path Configs::s_generatedPath = ("");

Configs& g_Configs() {
  return Configs::instance();
}

