#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <cuda_runtime.h>

class Vec3 : public float4 {
public:
    __host__ __device__ Vec3() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }
    
    __host__ __device__ Vec3(float e0, float e1, float e2) { 
        x = e0; y = e1; z = e2; w = 0.0f; 
    }
    
    __host__ __device__ inline float x_val() const { return x; }
    __host__ __device__ inline float y_val() const { return y; }
    __host__ __device__ inline float z_val() const { return z; }
    __host__ __device__ inline float r() const { return x; }
    __host__ __device__ inline float g() const { return y; }
    __host__ __device__ inline float b() const { return z; }

    __host__ __device__ inline const Vec3& operator+() const { return *this; }
    __host__ __device__ inline Vec3 operator-() const { return Vec3(-x, -y, -z); }
    
    // Subscript access mapped straight to the float4 properties
    __host__ __device__ inline float operator[](int i) const { 
        return (i == 0) ? x : (i == 1) ? y : z; 
    }
    __host__ __device__ inline float& operator[](int i) { 
        return (i == 0) ? x : (i == 1) ? y : z; 
    }

    __host__ __device__ inline Vec3& operator+=(const Vec3 &v2);
    __host__ __device__ inline Vec3& operator-=(const Vec3 &v2);
    __host__ __device__ inline Vec3& operator*=(const Vec3 &v2);
    __host__ __device__ inline Vec3& operator/=(const Vec3 &v2);
    __host__ __device__ inline Vec3& operator*=(const float t);
    __host__ __device__ inline Vec3& operator/=(const float t);

    // Fixed math calls to use precise __builtin math or fast intrinsics on device
    __host__ __device__ inline float length() const { return sqrtf(x*x + y*y + z*z); }
    __host__ __device__ inline float squared_length() const { return x*x + y*y + z*z; }
    
    __host__ __device__ inline Vec3 dot(const Vec3 v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }

    __host__ __device__ inline Vec3 cross(const Vec3 v2) const {
        return Vec3((y * v2.z - z * v2.y),
                    (-(x * v2.z - z * v2.x)),
                    (x * v2.y - y * v2.x));
    }

    __host__ __device__ inline Vec3 make_unit_vector() {
        float k = 1.0f / sqrtf(x*x + y*y + z*z);
        return Vec3(x * k, y * k, z * k);
    }

    __host__ __device__ inline void normalize() {
        float k = 1.0f / sqrtf(x*x + y*y + z*z);
        x *= k; y *= k; z *= k;
    }
};

// Stream operators updated to map straight to member fields
inline std::istream& operator>>(std::istream &is, Vec3 &t) {
    is >> t.x >> t.y >> t.z;
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const Vec3 &t) {
    os << t.x << " " << t.y << " " << t.z;
    return os;
}

__host__ __device__ inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

__host__ __device__ inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

__host__ __device__ inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

__host__ __device__ inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

__host__ __device__ inline Vec3 operator*(float t, const Vec3 &v) {
    return Vec3(t * v.x, t * v.y, t * v.z);
}

__host__ __device__ inline Vec3 operator/(Vec3 v, float t) {
    return Vec3(v.x / t, v.y / t, v.z / t);
}

__host__ __device__ inline Vec3 operator*(const Vec3 &v, float t) {
    return Vec3(t * v.x, t * v.y, t * v.z);
}

__host__ __device__ inline float dot(const Vec3 &v1, const Vec3 &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

__host__ __device__ inline Vec3 cross(const Vec3 &v1, const Vec3 &v2) {
    return Vec3((v1.y * v2.z - v1.z * v2.y),
                (-(v1.x * v2.z - v1.z * v2.x)),
                (v1.x * v2.y - v1.y * v2.x));
}

__host__ __device__ inline Vec3& Vec3::operator+=(const Vec3 &v){
    x += v.x; y += v.y; z += v.z;
    return *this;
}

__host__ __device__ inline Vec3& Vec3::operator*=(const Vec3 &v){
    x *= v.x; y *= v.y; z *= v.z;
    return *this;
}

__host__ __device__ inline Vec3& Vec3::operator/=(const Vec3 &v){
    x /= v.x; y /= v.y; z /= v.z;
    return *this;
}

__host__ __device__ inline Vec3& Vec3::operator-=(const Vec3& v) {
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

__host__ __device__ inline Vec3& Vec3::operator*=(const float t) {
    x *= t; y *= t; z *= t;
    return *this;
}

__host__ __device__ inline Vec3& Vec3::operator/=(const float t) {
    float k = 1.0f / t;
    x *= k; y *= k; z *= k;
    return *this;
}

__host__ __device__ inline Vec3 unit_vector(Vec3 v) {
    return v / v.length();
}

#endif