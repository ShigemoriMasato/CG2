#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <numbers>
#include "../Transform/Vector.h"
#include "../Transform/Matrix.h"
#include "../Transform/Transform.h"

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, float b);
Vector2 operator*(float a, const Vector2& b);
Vector2 operator/(const Vector2& a, const Vector2& b);
Vector2 operator/(const Vector2& a, float b);
Vector2 operator+=(Vector2& a, const Vector2& b);
Vector2 operator-=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, float b);
Vector2 operator/=(Vector2& a, const Vector2& b);
Vector2 operator/=(Vector2& a, float b);
bool operator==(const Vector2& a, const Vector2& b);
bool operator!=(const Vector2& a, const Vector2& b);

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a);
Vector3 operator*(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, float b);
Vector3 operator*(float a, const Vector3& b);
Vector3 operator/(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& a, float b);
Vector3 operator+=(Vector3& a, const Vector3& b);
Vector3 operator-=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, float b);
Vector3 operator/=(Vector3& a, const Vector3& b);
Vector3 operator/=(Vector3& a, float b);
bool operator==(const Vector3& a, const Vector3& b);
bool operator!=(const Vector3& a, const Vector3& b);

Vector4 operator+(const Vector4& a, const Vector4& b);
Vector4 operator-(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, float b);
Vector4 operator*(float a, const Vector4& b);
Vector4 operator/(const Vector4& a, const Vector4& b);
Vector4 operator/(const Vector4& a, float b);
Vector4 operator+=(Vector4& a, const Vector4& b);
Vector4 operator-=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, float b);
Vector4 operator/=(Vector4& a, const Vector4& b);
Vector4 operator/=(Vector4& a, float b);
bool operator==(const Vector4& a, const Vector4& b);
bool operator!=(const Vector4& a, const Vector4& b);

Matrix3x3 operator+(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator-(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);
Vector2 operator*(const Matrix3x3& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Matrix3x3& b);
Matrix3x3 operator/(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator+=(Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator-=(Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator*=(Matrix3x3& a, const Matrix3x3& b);
bool operator==(const Matrix3x3& a, const Matrix3x3& b);
bool operator!=(const Matrix3x3& a, const Matrix3x3& b);

Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
Vector3 operator*(const Matrix4x4& a, const Vector3& b);
Vector3 operator*(const Vector3& a, const Matrix4x4& b);
Matrix4x4 operator/(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator+=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator-=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator/=(Matrix4x4& a, const Matrix4x4& b);
bool operator==(const Matrix4x4& a, const Matrix4x4& b);
bool operator!=(const Matrix4x4& a, const Matrix4x4& b);

namespace Matrix {

	Matrix4x4 Inverse(const Matrix4x4& m);
	Matrix3x3 Inverse(const Matrix3x3& m);

	Matrix4x4 MakeTranslationMatrix(const Vector3& pos);
	Matrix4x4 MakeRotationMatrix(Vector3 angle);
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& angle, const Vector3& pos);
	Matrix4x4 MakeAffineMatrix(Transform transform);

	Matrix3x3 MakeTranslationMatrix(const Vector2& pos);
	Matrix3x3 MakeRotationMatrix(float angle);
	Matrix3x3 MakeScaleMatrix(const Vector2& scale);
	Matrix3x3 MakeAffineMatrix(const Vector2& scale, const float angle, const Vector2 translation);

	Matrix3x3 TransMatrix(const Matrix3x3& m);
	Matrix4x4 TransMatrix(const Matrix4x4& m);

	Matrix4x4 AdjustUVMatrix(Matrix3x3 m);

	Matrix3x3 MakeIdentity3x3();
	Matrix4x4 MakeIdentity4x4();


	Matrix4x4 MakeUVMatrix(Vector2 scale, float rotate, Vector2 position);
}

namespace MyMath {
	float lerp(float a, float b, float t);

	/// <summary>
	/// RGBAを線形補完
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	uint32_t lerp(uint32_t a, uint32_t b, float t);
	Vector3 lerp(const Vector3& a, const Vector3& b, float t);
	Quaternion lerp(const Quaternion& a, const Quaternion& b, float t);

	float EaseIn(float a, float b, float t);
	Vector3 EaseIn(Vector3 a, Vector3 b, float t);

	float EaseOut(float a, float b, float t);
	Vector3 EaseOut(Vector3 a, Vector3 b, float t);

	Vector3 Slerp(Vector3 a, Vector3 b, float t);

	float Repeat(float value, float max, float min = 0.0f);

	Vector3 ConvertVector(const Vector4& v);
	Vector4 ConvertVector(const Vector3& v, float w);

	float cot(float radian);

	Vector3 Normalize(Vector3 vec);

	Vector3 cross(const Vector3& a, const Vector3& b);
	float dot(const Vector3& a, const Vector3& b);
	float Length(const Vector3& vec);
}

std::vector<Vector3> GetSplinePoints(const std::vector<Vector3>& controlPoints, int segment);
std::vector<Vector3> GetCatmullPoints(const std::vector<Vector3>& points, int segmentsPerCurve);
Vector3 GetCatmullPoint(const std::vector<Vector3>& points, float t);
float CalculateAngle(const Vector3& a, const Vector3& b);

Vector4 ConvertColor(uint32_t color);
uint32_t ConvertColor(const Vector4& color);
