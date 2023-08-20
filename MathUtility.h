#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>
#include <cassert>

// ベクトルと行列の積
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// ノルム
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

// ビューポート変換
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// スケーリング行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 回転行列
Matrix4x4 MakeRotationXMatrix(float theta);
Matrix4x4 MakeRotationYMatrix(float theta);
Matrix4x4 MakeRotationZMatrix(float theta);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// アフィン行列
Matrix4x4 MakeAffinMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 演算子オーバーロード
Vector3& operator+=(Vector3& v, float s);
const Vector3 operator+(const Vector3& v, float s);

Vector3& operator-=(Vector3& v, float s);
const Vector3 operator-(const Vector3& v, float s);

Vector3& operator*=(Vector3& v, float s);
const Vector3 operator*(const Vector3& v, float s);

Vector3& operator/=(Vector3& v, float s);
const Vector3 operator/(const Vector3& v, float s);

Vector3& operator+=(Vector3& v1, const Vector3& v2);
const Vector3 operator+(const Vector3& v1, const Vector3& v2);

Vector3& operator-=(Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);

Matrix4x4& operator*=(Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);