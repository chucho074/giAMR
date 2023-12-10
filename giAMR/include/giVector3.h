/**
 * @file    giVector3.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    26/03/2021
 * @brief   A basic Vector 3.
 */
 
/**
 * @include
 */
#pragma once
#include "giPrerequisitesAMR.h"

namespace giAMRSDK {
  
  class Vector3
  {
   public:

    Vector3();
    
    Vector3(float inX, float inY, float inZ);
    
    ~Vector3();
   
    Vector3
    operator+(const Vector3& otherVect);
    
    Vector3
    operator-(const Vector3& otherVect);
    
    Vector3
    operator*(const Vector3& otherVect);
    
    Vector3
    operator/(const Vector3& otherVect); 

    void
    operator=(const Vector3& otherVect);
    
    void
    operator+=(const Vector3& otherVect);
    
    void
    operator-=(const Vector3& otherVect);
    
    void
    operator*=(const Vector3& otherVect);
    
    void
    operator/=(const Vector3& otherVect);
    
    Vector3
    operator+(const float& otherVal);
    
    Vector3
    operator-(const float& otherVal);
    
    Vector3
    operator*(const float& otherVal);
    
    Vector3
    operator/(const float& otherVal);
    
    bool
    operator>=(const Vector3& otherVect);
    
    bool
    operator<=(const Vector3& otherVect);
    
    bool
    operator==(const Vector3& otherVect);
    
    bool
    operator<(const Vector3& otherVect);
    
    bool
    operator>(const Vector3& otherVect);
    
    bool
    operator>=(const float& otherVal);
    
    bool
    operator<=(const float& otherVal);
    
    bool
    operator==(const float& otherVal);
    
    bool
    operator<(const float& otherVal);
    
    bool
    operator>(const float& otherVal);
    
    float
    dotProd(const Vector3& inVect);
    
    Vector3
    crossProd(const Vector3& inVect);
    
    Vector3
    normalize();
    
    float
    magnitude();
    
   
    float x;
    float y;
    float z;

    /**
     * @brief A zero vector (0,0,0)
     */
    static const Vector3 ZERO;

    /**
     * @brief A unit vector (1,1,1)
     */
    static const Vector3 UNIT;

  };
  
}