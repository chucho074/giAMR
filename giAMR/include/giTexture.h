/**
 * @file    giTexture.h
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

#include <d3d11.h>

namespace giAMRSDK {

  namespace TEXTURE_TYPE {
    enum E {
      kUnknown = 0,
      kAlbedo,
      kMetalic,
      kRoughness,
      kAmbientOclussion,
      kNormal,
      kGloss,
      kSpecular,
      kOpacity,
      kSuportedTextureTypes
    };
  }


  class Texture
  {
  public:
    Texture() = default;

    ~Texture() = default;

    //The name of the Texture.
    String m_name;

    //The type of the Texture.
    TEXTURE_TYPE::E m_type = TEXTURE_TYPE::kUnknown;

    //The path of the file.
    Path m_path;

    ID3D11Texture2D* m_texture = nullptr;
  };
}