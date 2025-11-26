#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

class Camera;

struct Transform
{
	Vector3 scale{ 1.0f, 1.0f, 1.0f }; //拡大縮小
	Vector3 rotation{ 0.0f, 0.0f, 0.0f }; //回転
	Vector3 position{ 0.0f, 0.0f, 0.0f }; //位置
};


struct VertexData final
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct VertexTexture final {
	uint32_t textureIndex;
};

struct ModelVertexData final
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	uint32_t textureIndex_;
	int nodeIndex;
};

struct Material final
{
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};
	float shininess = 0.0f;
};

struct ModelParticleData final
{
	Matrix4x4 wvp;
	Matrix4x4 world;
	Vector4 color;
	int textureIndex;
	Vector3 worldPos;
};

struct MatrixData final
{
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct DirectionalLightData final
{
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 direction = { -0.68f, 0.72f, -0.12f };
	float intensity = 1.0f;
	int32_t enableLighting = 0;
	Vector3 padding;
};

struct PointLightData final
{
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	float intensity = 1.0f;
};

//即興クオータニオン。仕組みはよくわかってない。AI制なので、いい感じに変えて
struct Quaternion {
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

	// 単位クオータニオン生成（回転なし）
	static Quaternion Identity() {
		return { 0, 0, 0, 1 };
	}

	// 軸と角度から生成
	static Quaternion FromAxisAngle(const Vector3& axis, float angle) {
		float half = angle * 0.5f;
		float s = sin(half);
		return {
			axis.x * s,
			axis.y * s,
			axis.z * s,
			cos(half)
		};
	}

	// 正規化
	Quaternion Normalize() const {
		float len = sqrt(x * x + y * y + z * z + w * w);
		if (len == 0.0f) {
			return { 0, 0, 0, 1 };
		}
		return { x / len, y / len, z / len, w / len };
	}

	Matrix4x4 ToMatrix() const {
		// 正規化
		Quaternion a = Normalize();

		float x2 = a.x * a.x;
		float y2 = a.y * a.y;
		float z2 = a.z * a.z;
		float w2 = a.w * a.w;

		return {
			w2 + x2 - y2 - z2, 2 * (a.x * a.y + a.w * a.z), 2 * (a.x * a.z - a.w * a.y), 0,
			2 * (a.x * a.y - a.w * a.z), w2 - x2 + y2 - z2, 2 * (a.y * a.z + a.w * a.x), 0,
			2 * (a.x * a.z + a.w * a.y), 2 * (a.y * a.z - a.w * a.x), w2 - x2 - y2 + z2, 0,
			0, 0, 0, 1
		};
	}
};

struct AABB final
{
	Vector3 min{ 0.0f, 0.0f, 0.0f };
	Vector3 max{ 0.0f, 0.0f, 0.0f };

	Vector3 Position()const
	{
		Vector3 pos;
		pos.x = (min.x + max.x) * 0.5f;
		pos.y = (min.y + max.y) * 0.5f;
		pos.z = (min.z + max.z) * 0.5f;
		return pos;
	}
	Vector3 Size()const
	{
		Vector3 size;
        size.x = max.x - min.x;
        size.y = max.y - min.y;
        size.z = max.z - min.z;
		return size;
	}
};

struct Sphere
{
	Vector3 center;
	float radius;
};
