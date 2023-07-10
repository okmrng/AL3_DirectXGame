#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

/// <summary>
/// 線形補間
/// </summary>
/// <param name="v1">スタート</param>/// <param name="v2">ゴール</param>/// <param name="t">フレーム</param>/// <returns></returns>
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// ノルム
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

Vector3& operator-=(Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);

Vector3& operator*=(Vector3& v, float s);
const Vector3 operator*(const Vector3& v, float s);