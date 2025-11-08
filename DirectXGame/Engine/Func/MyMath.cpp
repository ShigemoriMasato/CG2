#include "MyMath.h"
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace MyMath;
using namespace Matrix;

namespace
{
	bool ZeroChecker(float* value, int size)
	{
		bool isNotZero = true;

		for (int i = 0; i < size; ++i)
		{
			if (std::fabsf(value[i]) <= 1e-6)
			{
				isNotZero = false;
				break;
			}
		}

		return isNotZero;
	}
}

Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 operator*(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x * b.x, a.y * b.y);
}

Vector2 operator*(const Vector2& a, float b)
{
	return Vector2(a.x * b, a.y * b);
}

Vector2 operator*(float a, const Vector2& b)
{
	return Vector2(a * b.x, a * b.y);
}

Vector2 operator/(const Vector2& a, const Vector2& b)
{
	if (!ZeroChecker((float*)&b, 2))
	{
		assert(false && "Division by zero in Vector2 operator/");
	}
	return Vector2(a.x / b.x, a.y / b.y);
}

Vector2 operator/(const Vector2& a, float b)
{
	if (std::fabsf(b) <= 1e-6)
	{
		assert(false && "Division by zero in Vector2 operator/");
	}
	return Vector2(a.x / b, a.y / b);
}

Vector2 operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

Vector2 operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

Vector2 operator*=(Vector2& a, const Vector2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

Vector2 operator*=(Vector2& a, float b)
{
	a.x *= b;
	a.y *= b;
	return a;
}

Vector2 operator/=(Vector2& a, const Vector2& b)
{
	if (!ZeroChecker((float*)&b, 2))
	{
		assert(false && "Division by zero in Vector2 operator/=");
	}
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

Vector2 operator/=(Vector2& a, float b)
{
	if (std::fabsf(b) <= 1e-6)
	{
		assert(false && "Division by zero in Vector2 operator/=");
	}
	a.x /= b;
	a.y /= b;
	return a;
}

bool operator==(const Vector2& a, const Vector2& b)
{
	return a.x == b.x && a.y == b.y;
}

bool operator!=(const Vector2& a, const Vector2& b)
{
	return !(a == b);
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator-(const Vector3& a)
{
	return Vector3(-a.x, -a.y, -a.z);
}

Vector3 operator*(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator*(const Vector3& a, float b)
{
	return Vector3(a.x * b, a.y * b, a.z * b);
}

Vector3 operator*(float a, const Vector3& b)
{
	return Vector3(a * b.x, a * b.y, a * b.z);
}

Vector3 operator/(const Vector3& a, const Vector3& b)
{
	if (!ZeroChecker((float*)&b, 3))
	{
		assert(false && "Division by zero in Vector3 operator/");
	}
	return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3 operator/(const Vector3& a, float b)
{
	if (!ZeroChecker(&b, 1))
	{
		assert(false && "Division by zero in Vector3 operator/");
	}
	return Vector3(a.x / b, a.y / b, a.z / b);
}

Vector3 operator+=(Vector3& a, const Vector3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vector3 operator-=(Vector3& a, const Vector3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vector3 operator*=(Vector3& a, const Vector3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}

Vector3 operator*=(Vector3& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}

Vector3 operator/=(Vector3& a, const Vector3& b)
{
	if (!ZeroChecker((float*)&b, 3))
	{
		assert(false && "Division by zero in Vector3 operator/=");
	}
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}

Vector3 operator/=(Vector3& a, float b)
{
	if (!ZeroChecker(&b, 1))
	{
		assert(false && "Division by zero in Vector3 operator/=");
	}
	a.x /= b;
	a.y /= b;
	a.z /= b;
	return a;
}

bool operator==(const Vector3& a, const Vector3& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const Vector3& a, const Vector3& b)
{
	return !(a == b);
}

Vector4 operator+(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vector4 operator*(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 operator*(const Vector4& a, float b)
{
	return Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
}

Vector4 operator*(float a, const Vector4& b)
{
	return Vector4(a * b.x, a * b.y, a * b.z, a * b.w);
}

Vector4 operator/(const Vector4& a, const Vector4& b)
{
	if (!ZeroChecker((float*)&b, 4))
	{
		assert(false && "Division by zero in Vector4 operator/");
	}
	return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

Vector4 operator/(const Vector4& a, float b)
{
	if (!ZeroChecker(&b, 1))
	{
		assert(false && "Division by zero in Vector4 operator/");
	}
	return Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
}

Vector4 operator+=(Vector4& a, const Vector4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

Vector4 operator-=(Vector4& a, const Vector4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

Vector4 operator*=(Vector4& a, const Vector4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}

Vector4 operator*=(Vector4& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}

Vector4 operator/=(Vector4& a, const Vector4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}

Vector4 operator/=(Vector4& a, float b)
{
	a.x /= b;
	a.y /= b;
	a.z /= b;
	a.w /= b;
	return a;
}

bool operator==(const Vector4& a, const Vector4& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool operator!=(const Vector4& a, const Vector4& b)
{
	return !(a == b);
}

Matrix3x3 operator+(const Matrix3x3& a, const Matrix3x3& b)
{
	Matrix3x3 result;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			result.m[i][j] = a.m[i][j] + b.m[i][j];
		}
	}
	return result;
}

Matrix3x3 operator-(const Matrix3x3& a, const Matrix3x3& b)
{
	Matrix3x3 result;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			result.m[i][j] = a.m[j][i] - b.m[i][j];
		}
	}
	return result;
}

Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b)
{
	Matrix3x3 result = {};
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				result.m[j][i] += a.m[k][i] * b.m[j][k];
			}
		}
	}
	return result;
}

Vector2 operator*(const Matrix3x3& m, const Vector2& v)
{
	Vector2 ans;
	ans.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2];
	ans.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2];
	float w = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2];
	if (std::fabsf(w) <= 1e-6)
	{
		assert(false && "Division by zero in Vector2 operator* with Matrix3x3");
	}
	ans.x /= w;
	ans.y /= w;
	return ans;
}

Vector2 operator*(const Vector2& a, const Matrix3x3& b)
{
	return b * a;
}

Matrix3x3 operator/(const Matrix3x3& a, const Matrix3x3& b)
{
	if (ZeroChecker((float*)&b, 9))
	{
		assert(false && "Division by zero in Matrix3x3 operator/");
	}
	return a * Inverse(b);
}

Matrix3x3 operator+=(Matrix3x3& a, const Matrix3x3& b)
{
	a = a + b;
	return a;
}

Matrix3x3 operator-=(Matrix3x3& a, const Matrix3x3& b)
{
	a = a - b;
	return a;
}

Matrix3x3 operator*=(Matrix3x3& a, const Matrix3x3& b)
{
	a = a * b;
	return a;
}

Matrix3x3 operator/=(Matrix3x3& a, const Matrix3x3& b)
{
	if (ZeroChecker((float*)&b, 9))
	{
		assert(false && "Division by zero in Matrix3x3 operator/=");
	}
	a = a / b;
	return a;
}

bool operator==(const Matrix3x3& a, const Matrix3x3& b)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (a.m[i][j] != b.m[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

bool operator!=(const Matrix3x3& a, const Matrix3x3& b)
{
	return !(a == b);
}

Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = a.m[i][j] + b.m[i][j];
		}
	}
	return result;
}

Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = a.m[i][j] - b.m[i][j];
		}
	}
	return result;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return result;
}

Vector3 operator*(const Matrix4x4& mat, const Vector3& vec)
{
	Vector3 ans;
	ans.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
	ans.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
	ans.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];
	float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3];
	assert(w != 0.0f);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;
	return ans;
}

Vector3 operator*(const Vector3& a, const Matrix4x4& b)
{
	return b * a;
}

Matrix4x4 operator/(const Matrix4x4& a, const Matrix4x4& b)
{
	if (ZeroChecker((float*)&b, 16))
	{
		assert(false && "Division by zero in Matrix4x4 operator/");
	}
	return a * Inverse(b);
}

Matrix4x4 operator+=(Matrix4x4& a, const Matrix4x4& b)
{
	a = a + b;
	return a;
}

Matrix4x4 operator-=(Matrix4x4& a, const Matrix4x4& b)
{
	a = a - b;
	return a;
}

Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b)
{
	a = a * b;
	return a;
}

Matrix4x4 operator/=(Matrix4x4& a, const Matrix4x4& b)
{
	a = a * Inverse(b);
	return a;
}

bool operator==(const Matrix4x4& a, const Matrix4x4& b)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (a.m[i][j] != b.m[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

bool operator!=(const Matrix4x4& a, const Matrix4x4& b)
{
	return !(a == b);
}

Vector3 deCasteljau(const std::vector<Vector3>& controlPoints, float t)
{
	if (controlPoints.size() == 1) return controlPoints[0];

	std::vector<Vector3> nextLevel;
	for (size_t i = 0; i < controlPoints.size() - 1; ++i)
	{
		Vector3 p = controlPoints[i] * (1 - t) + controlPoints[i + 1] * t;
		nextLevel.push_back(p);
	}

	return deCasteljau(nextLevel, t);
}

std::vector<Vector3> GetSplinePoints(const std::vector<Vector3>& controlPoints, int segment)
{
	std::vector<Vector3> ans;
	for (int i = 0; i < segment; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(segment);

		ans.push_back(deCasteljau(controlPoints, t));
	}

	return ans;
}

Vector3 catmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	return
		p0 * (-0.5f * t3 + t2 - 0.5f * t) +
		p1 * (1.5f * t3 - 2.5f * t2 + 1.0f) +
		p2 * (-1.5f * t3 + 2.0f * t2 + 0.5f * t) +
		p3 * (0.5f * t3 - 0.5f * t2);
}

std::vector<Vector3> GetCatmullPoints(const std::vector<Vector3>& points, int segmentsPerCurve)
{
	std::vector<Vector3> ans;

	if (points.size() < 4) return ans;

	for (size_t i = 1; i < points.size() - 2; ++i)
	{
		for (int j = 0; j <= segmentsPerCurve; ++j)
		{
			float t = static_cast<float>(j) / segmentsPerCurve;
			ans.push_back(catmullRom(points[i - 1], points[i], points[i + 1], points[i + 2], t));
		}
	}

	return ans;
}

Vector3 GetCatmullPoint(const std::vector<Vector3>& points, float t)
{
	if (points.size() < 4)
	{
		assert(false && "Not enough points for Catmull-Rom interpolation");
		return Vector3();
	}
	float segmentLength = 1.0f / (points.size() - 3);
	int segmentIndex = static_cast<int>(t / segmentLength);
	segmentIndex = std::clamp(segmentIndex, 0, static_cast<int>(points.size()) - 4);
	float localT = (t - segmentIndex * segmentLength) / segmentLength;
	return catmullRom(points[segmentIndex], points[segmentIndex + 1], points[segmentIndex + 2], points[segmentIndex + 3], localT);
}

float CalculateAngle(const Vector3& a, const Vector3& b)
{
	float dotProduct = dot(a,b);
	float magnitudeA = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	float magnitudeB = std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);

	if (magnitudeA == 0 || magnitudeB == 0) return (float)std::numbers::pi;

	float cosAngle = dotProduct / (magnitudeA * magnitudeB);
	cosAngle = std::clamp(cosAngle, -1.0f, 1.0f);

	return std::cos(cosAngle);
}

Vector4 ConvertColor(uint32_t color) {
	Vector4 converted = {
		((color >> 24) & 0xff) / 255.0f,
		((color >> 16) & 0xff) / 255.0f,
		((color >> 8) & 0xff) / 255.0f,
		((color >> 0) & 0xff) / 255.0f
	};
	return converted;
}

float MyMath::lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

uint32_t MyMath::lerp(uint32_t start, uint32_t end, float t)
{
	int sa = start & 0xff;
	start >>= 8;
	int sb = start & 0xff;
	start >>= 8;
	int sg = start & 0xff;
	start >>= 8;
	int sr = start & 0xff;

	int ea = end & 0xff;
	end >>= 8;
	int eb = end & 0xff;
	end >>= 8;
	int eg = end & 0xff;
	end >>= 8;
	int er = end & 0xff;

	int aa = static_cast<int>(sa + (ea - sa) * t) << 24;
	int ab = static_cast<int>(sb + (eb - sb) * t) << 16;
	int ag = static_cast<int>(sg + (eg - sg) * t) << 8;
	int ar = static_cast<int>(sr + (er - sr) * t);

	return aa | ab | ag | ar;
}

Vector3 MyMath::lerp(const Vector3& a, const Vector3& b, float t)
{
	return Vector3(
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t)
	);
}

Quaternion MyMath::lerp(const Quaternion& a, const Quaternion& b, float t) {
	return Quaternion(
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t),
		lerp(a.w, b.w, t)
	);
}

float MyMath::EaseIn(float a, float b, float t)
{
	return a + (b - a) * t * t * t; // Cubic ease-in
}

Vector3 MyMath::EaseIn(Vector3 a, Vector3 b, float t)
{
	return Vector3(
		a.x + (b.x - a.x) * t * t * t,
		a.y + (b.y - a.y) * t * t * t,
		a.z + (b.z - a.z) * t * t * t
	);
}

float MyMath::EaseOut(float a, float b, float t)
{
	t = 1.0f - t; // Invert t for ease-out
	return a + (b - a) * (1.0f - t * t * t); // Cubic ease-out
}

Vector3 MyMath::EaseOut(Vector3 a, Vector3 b, float t)
{
	t = 1.0f - t; // Invert t for ease-out
	return Vector3(
		a.x + (b.x - a.x) * (1.0f - t * t * t),
		a.y + (b.y - a.y) * (1.0f - t * t * t),
		a.z + (b.z - a.z) * (1.0f - t * t * t)
	);
}

Vector3 MyMath::Slerp(Vector3 a, Vector3 b, float t)
{
	if (t <= 0.0f) return a;
	if (t >= 1.0f) return b;
	float dotProduct = std::clamp(dot(a, b), 0.0f, 1.0f);
	float theta = std::acosf(dotProduct);
	float sinTheta = std::sinf(theta);
	if (sinTheta < 1e-6f)
	{
		return a;
	}
	float ratioA = std::sinf((1.0f - t) * theta) / sinTheta;
	float ratioB = std::sinf(t * theta) / sinTheta;
	return Vector3(
		a.x * ratioA + b.x * ratioB,
		a.y * ratioA + b.y * ratioB,
		a.z * ratioA + b.z * ratioB
	);
}

float MyMath::Repeat(float value, float max, float min)
{
	while (value < min || value > max)
	{
		if (value < min)
		{
			value += (max - min);
		}
		else if (value > max)
		{
			value -= (max - min);
		}
	}
	return value;
}

Vector3 MyMath::ConvertVector(const Vector4& v)
{
	return Vector3(v.x, v.y, v.z);
}

Vector4 MyMath::ConvertVector(const Vector3& v, float w)
{
	return Vector4(v.x, v.y, v.z, w);
}

float MyMath::cot(float radian)
{
	return std::cosf(radian) / std::sinf(radian);
}

Vector3 MyMath::Normalize(Vector3 vec)
{
	float length = std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	return { vec.x / length, vec.y / length, vec.z / length };
}

Vector3 MyMath::cross(const Vector3& a, const Vector3& b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float MyMath::dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float MyMath::Length(const Vector3& vec)
{
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Matrix3x3 Matrix::Inverse(const Matrix3x3& mat)
{
	float d = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] + mat.m[0][2] * mat.m[1][0] * mat.m[2][1] -
		mat.m[0][2] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1];

	Matrix3x3 ans = { mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1] / d,
		-mat.m[0][1] * mat.m[2][2] + mat.m[0][2] * mat.m[2][1] / d,
		mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1] / d,

		-mat.m[1][0] * mat.m[2][2] + mat.m[1][2] * mat.m[2][0] / d,
		mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0] / d,
		-mat.m[0][0] * mat.m[1][2] + mat.m[0][2] * mat.m[1][0] / d,

		mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0] / d,
		-mat.m[0][0] * mat.m[2][1] + mat.m[0][1] * mat.m[2][0] / d,
		mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0] / d
	};

	return mat;
}

Matrix3x3 Matrix::MakeIdentity3x3()
{
	return { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
}

Matrix3x3 Matrix::MakeTranslationMatrix(const Vector2& translation)
{
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		translation.x, translation.y, 1.0f
	};
}

Matrix3x3 Matrix::MakeRotationMatrix(float angle)
{
	return {
		std::cosf(angle), std::sinf(angle), 0.0f,
		-sinf(angle), std::cosf(angle), 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeScaleMatrix(const Vector2& scale)
{
	return {
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeAffineMatrix(const Vector2& scale, const float angle, const Vector2 translation)
{
	return MakeScaleMatrix(scale) * MakeRotationMatrix(angle) * MakeTranslationMatrix(translation);
}

Matrix3x3 Matrix::TransMatrix(const Matrix3x3& m)
{
	Matrix3x3 ans{};
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			ans.m[j][i] = m.m[i][j];
		}
	}
	return ans;
}

Matrix4x4 Matrix::TransMatrix(const Matrix4x4& m)
{
	Matrix4x4 ans{};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ans.m[j][i] = m.m[i][j];
		}
	}
	return ans;
}

Matrix4x4 Matrix::AdjustUVMatrix(Matrix3x3 m)
{
	Matrix4x4 ans = {
		m.m[0][0], m.m[0][1], 0.0f, 0.0f,
		m.m[1][0], m.m[1][1], 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		m.m[2][0], m.m[2][1], 0.0f, 1.0f
	};

	return TransMatrix(ans);
}

Matrix4x4 Matrix::Inverse(const Matrix4x4& mat)
{
	float d = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3]
		+ mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1]
		+ mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
		- mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1]
		- mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3]
		- mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2]
		- mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3]
		- mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1]
		- mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2]
		+ mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1]
		+ mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3]
		+ mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2]
		+ mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3]
		+ mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
		+ mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2]
		- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1]
		- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3]
		- mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
		- mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0]
		- mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0]
		- mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0]
		+ mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0]
		+ mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0]
		+ mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0];

	return {
		(mat.m[1][1] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][1] + mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
			- mat.m[1][3] * mat.m[2][2] * mat.m[3][1] - mat.m[1][2] * mat.m[2][1] * mat.m[3][3] - mat.m[1][1] * mat.m[2][3] * mat.m[3][2]) / d,
		-(mat.m[0][1] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][1] + mat.m[0][3] * mat.m[2][1] * mat.m[3][2]
			- mat.m[0][3] * mat.m[2][2] * mat.m[3][1] - mat.m[0][2] * mat.m[2][1] * mat.m[3][3] - mat.m[0][1] * mat.m[2][3] * mat.m[3][2]) / d,
		(mat.m[0][1] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][1] + mat.m[0][3] * mat.m[1][1] * mat.m[3][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[3][1] - mat.m[0][2] * mat.m[1][1] * mat.m[3][3] - mat.m[0][1] * mat.m[1][3] * mat.m[3][2]) / d,
		-(mat.m[0][1] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][1] + mat.m[0][3] * mat.m[1][1] * mat.m[2][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[2][1] - mat.m[0][2] * mat.m[1][1] * mat.m[2][3] - mat.m[0][1] * mat.m[1][3] * mat.m[2][2]) / d,

		-(mat.m[1][0] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][0] + mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
			- mat.m[1][3] * mat.m[2][2] * mat.m[3][0] - mat.m[1][2] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][2]) / d,
		(mat.m[0][0] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][0] + mat.m[0][3] * mat.m[2][0] * mat.m[3][2]
			- mat.m[0][3] * mat.m[2][2] * mat.m[3][0] - mat.m[0][2] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][2]) / d,
		-(mat.m[0][0] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][0] + mat.m[0][3] * mat.m[1][0] * mat.m[3][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[3][0] - mat.m[0][2] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][2]) / d,
		(mat.m[0][0] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][0] + mat.m[0][3] * mat.m[1][0] * mat.m[2][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] - mat.m[0][2] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][2]) / d,

		(mat.m[1][0] * mat.m[2][1] * mat.m[3][3] + mat.m[1][1] * mat.m[2][3] * mat.m[3][0] + mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
			- mat.m[1][3] * mat.m[2][1] * mat.m[3][0] - mat.m[1][1] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[2][1] * mat.m[3][3] + mat.m[0][1] * mat.m[2][3] * mat.m[3][0] + mat.m[0][3] * mat.m[2][0] * mat.m[3][1]
			- mat.m[0][3] * mat.m[2][1] * mat.m[3][0] - mat.m[0][1] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[1][1] * mat.m[3][3] + mat.m[0][1] * mat.m[1][3] * mat.m[3][0] + mat.m[0][3] * mat.m[1][0] * mat.m[3][1]
			- mat.m[0][3] * mat.m[1][1] * mat.m[3][0] - mat.m[0][1] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[1][1] * mat.m[2][3] + mat.m[0][1] * mat.m[1][3] * mat.m[2][0] + mat.m[0][3] * mat.m[1][0] * mat.m[2][1]
			- mat.m[0][3] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][1]) / d,

		-(mat.m[1][0] * mat.m[2][1] * mat.m[3][2] + mat.m[1][1] * mat.m[2][2] * mat.m[3][0] + mat.m[1][2] * mat.m[2][0] * mat.m[3][1]
			- mat.m[1][2] * mat.m[2][1] * mat.m[3][0] - mat.m[1][1] * mat.m[2][0] * mat.m[3][2] - mat.m[1][0] * mat.m[2][2] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[2][1] * mat.m[3][2] + mat.m[0][1] * mat.m[2][2] * mat.m[3][0] + mat.m[0][2] * mat.m[2][0] * mat.m[3][1]
			- mat.m[0][2] * mat.m[2][1] * mat.m[3][0] - mat.m[0][1] * mat.m[2][0] * mat.m[3][2] - mat.m[0][0] * mat.m[2][2] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[1][1] * mat.m[3][2] + mat.m[0][1] * mat.m[1][2] * mat.m[3][0] + mat.m[0][2] * mat.m[1][0] * mat.m[3][1]
			- mat.m[0][2] * mat.m[1][1] * mat.m[3][0] - mat.m[0][1] * mat.m[1][0] * mat.m[3][2] - mat.m[0][0] * mat.m[1][2] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] + mat.m[0][2] * mat.m[1][0] * mat.m[2][1]
			- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1]) / d
	};
}

Matrix4x4 Matrix::MakeIdentity4x4()
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeUVMatrix(Vector2 scale, float rotate, Vector2 position) {
	return MakeScaleMatrix(Vector3(scale.x, scale.y, 1.0f)) *
		MakeRotationMatrix(Vector3(0.0f, 0.0f, rotate)) *
		MakeTranslationMatrix(Vector3(position.x, position.y, 0.0f));
}

Matrix4x4 Matrix::MakeTranslationMatrix(const Vector3& pos)
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, pos.z, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotationMatrix(Vector3 angle)
{
	Matrix4x4 x = {
		1.0f ,0.0f, 0.0f, 0.0f,
		0.0f, std::cosf(angle.x), std::sinf(angle.x), 0.0f,
		0.0f, -std::sinf(angle.x), std::cosf(angle.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4x4 y = {
		std::cosf(angle.y), 0.0f, -std::sinf(angle.y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sinf(angle.y), 0.0f, std::cosf(angle.y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4x4 z = {
		std::cosf(angle.z), std::sinf(angle.z), 0.0f, 0.0f,
		-sinf(angle.z), std::cosf(angle.z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return x * y * z;
}

Matrix4x4 Matrix::MakeScaleMatrix(const Vector3& scale)
{
	return {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation)
{
	return MakeScaleMatrix(scale) *
		MakeRotationMatrix(rotation) *
		MakeTranslationMatrix(translation);
}

Matrix4x4 Matrix::MakeAffineMatrix(Transform transform)
{
	return MakeAffineMatrix(transform.position, transform.rotation, transform.scale);
}

bool CollisionChecker(AABB a, AABB b)
{
	return (a.min.x <= b.max.x && a.max.x >= b.min.x &&
		a.min.y <= b.max.y && a.max.y >= b.min.y &&
		a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool CollisionChecker(Sphere a, Sphere b)
{
	float distance = Vector3(a.center - b.center).Length();
	return distance <= (a.radius + b.radius);
}
