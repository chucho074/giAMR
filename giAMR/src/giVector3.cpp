/**
 * @file    giVector3.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    10/05/2023
 * @brief   A basic description of the what do the doc.
 */
 
/**
 * @include
 */
#include "giVector3.h"

namespace giAMRSDK {

  const Vector3 Vector3::ZERO = Vector3(0.f, 0.f, 0.f);
  const Vector3 Vector3::UNIT = Vector3(1.f, 1.f, 1.f);

  Vector3::Vector3() {
    *this = ZERO;
  }

  Vector3::Vector3(float inX, float inY, float inZ) {
    x = inX;
    y = inY;
    z = inZ;
  }
  
  Vector3::~Vector3() {
  
  }
  
  Vector3
  Vector3::operator+(const Vector3& otherVect) {
    return Vector3(x + otherVect.x, y + otherVect.y, z + otherVect.z);
  }
    
  Vector3
  Vector3::operator-(const Vector3& otherVect) {
    return Vector3(x - otherVect.x, y - otherVect.y, z - otherVect.z);
  }
    
  Vector3
  Vector3::operator*(const Vector3& otherVect) {
    return Vector3(x * otherVect.x, y * otherVect.y, z * otherVect.z);
  }
    
  Vector3
  Vector3::operator/(const Vector3& otherVect) {
    return Vector3(x / otherVect.x, y / otherVect.y, z / otherVect.z);
  }

  void 
  Vector3::operator=(const Vector3& otherVect) {
    x = otherVect.x;
    y = otherVect.y;
    z = otherVect.z;
  }

  void 
  Vector3::operator+=(const Vector3& otherVect) {
    x += otherVect.x;
    y += otherVect.y;
    z += otherVect.z;
  }

  void 
  Vector3::operator-=(const Vector3& otherVect) {
    x -= otherVect.x;
    y -= otherVect.y;
    z -= otherVect.z;
  }

  void 
  Vector3::operator*=(const Vector3& otherVect) {
    x *= otherVect.x;
    y *= otherVect.y;
    z *= otherVect.z;
  }

  void 
  Vector3::operator/=(const Vector3& otherVect) {
    x /= otherVect.x;
    y /= otherVect.y;
    z /= otherVect.z;
  }
    
  Vector3
  Vector3::operator+(const float& otherVal) {
    return Vector3(x + otherVal, y + otherVal, z + otherVal);
  }
    
  Vector3
  Vector3::operator-(const float& otherVal) {
    return Vector3(x - otherVal, y - otherVal, z - otherVal);
  }
    
  Vector3
  Vector3::operator*(const float& otherVal) {
    return Vector3(x * otherVal, y * otherVal, z * otherVal);
  }
    
  Vector3
  Vector3::operator/(const float& otherVal) {
    return Vector3(x / otherVal, y / otherVal, z / otherVal);
  }
  
  bool
  Vector3::operator>=(const Vector3& otherVect) {
    if (otherVect.x <= x && otherVect.y <= y && otherVect.z <= z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator<=(const Vector3& otherVect) {
    if (otherVect.x >= x && otherVect.y >= y && otherVect.z >= z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator==(const Vector3& otherVect) {
    if (otherVect.x == x && otherVect.y == y && otherVect.z == z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator<(const Vector3& otherVect) {
    if (otherVect.x > x && otherVect.y > y && otherVect.z > z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator>(const Vector3& otherVect) {
    if (otherVect.x < x && otherVect.y < y && otherVect.z < z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator>=(const float& otherVal) {
    if (otherVal <= x && otherVal <= y && otherVal <= z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator<=(const float& otherVal) {
    if (otherVal >= x && otherVal >= y && otherVal >= z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator==(const float& otherVal) {
    if (otherVal == x && otherVal == y && otherVal == z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator<(const float& otherVal) {
    if (otherVal > x && otherVal > y && otherVal > z) {
      return true;
    }
    return false;
  }
  
  bool
  Vector3::operator>(const float& otherVal) {
    if (otherVal < x && otherVal < y && otherVal < z) {
      return true;
    }
    return false;
  }
  
  float
  Vector3::dotProd(const Vector3& inVect) {
    return ((x * inVect.x) + (y * inVect.y)+ (z * inVect.z));
  }
  
  Vector3
  Vector3::crossProd(const Vector3& inVect) {
    return {((y * inVect.z) - (z * inVect.y)),
            ((z * inVect.x) - (x * inVect.z)),
            ((x * inVect.y) - (y * inVect.z))};
  }
  
  Vector3
  Vector3::normalize() {
    Vector3 tmpOut;
    float mag = magnitude();
    if (mag > 0) {
      float tmp = 1.0f / mag;
      tmpOut.x *= tmp;
      tmpOut.y *= tmp;
      tmpOut.z *= tmp;
    }
    else {
      tmpOut.x = 0.0f;
      tmpOut.y = 0.0f;
      tmpOut.z = 0.0f;
    }

    return tmpOut;
  }
  
  float
  Vector3::magnitude() {
    return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
  }

}