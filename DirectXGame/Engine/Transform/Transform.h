#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

//Shader
#include <../Assets/Shader/ShaderCommon.hlsli>
#include <../Assets/Shader/Particle.hlsli>
#include <../Assets/Shader/Reflection.hlsli>

//hlsliをcppで読み込めるようにするためのやつ
#ifdef __cplusplus
using float4 = Vector4;
using float3 = Vector3;
using float2 = Vector2;
using float4x4 = Matrix4x4;
using float3x3 = Matrix3x3;
#endif

struct Transform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f }; //拡大縮小
	Vector3 rotation{ 0.0f, 0.0f, 0.0f }; //回転
	Vector3 position{ 0.0f, 0.0f, 0.0f }; //位置
};

struct AABB final {
	Vector3 min{ 0.0f, 0.0f, 0.0f };
	Vector3 max{ 0.0f, 0.0f, 0.0f };
};

struct Sphere {
	Vector3 center;
	float radius;
};