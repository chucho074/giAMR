/**
 * @file    giImporter.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    08/05/2023
 * @brief   A basic implementation for import other types 
             of models diferent than obj.
 */
 
/**
 * @include
 */
#pragma once
#include "giPrerequisitesAMR.h"
#include "giModel.h"

namespace giAMRSDK {
  class Importer
  {
   public:
  	Importer() = default;
  	~Importer() = default;
  
    /**
     * @brief    Reads the basic information of a model and returns it.
     */
    static void
    readBasicModel(Path & inFile, ModelInfo& inInfo);
  
  };
}