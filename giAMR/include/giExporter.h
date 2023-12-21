/**
 * @file    giExporter.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    08/05/2023
 * @brief   An exporter for the data used.
 */
 
/**
 * @include
 */
#pragma once
#include "giPrerequisitesAMR.h"
#include "giModel.h"

namespace giAMRSDK {
  struct ModelInfo;
 
  class Exporter
  {
  public:
    Exporter() = default;
    ~Exporter() = default;

    static void
    ExportObj(Path inPath, SharedPtr<Model> inModel);
    
    static void
    ExportMtl(Path inPath);

    static Path
    ExportAsObj(Path inPath, String inFileType);

  };

}