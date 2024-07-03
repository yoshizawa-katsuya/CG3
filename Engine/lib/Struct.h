#pragma once
#include <cstdint>
#include <vector>
#include <string>

//static const int kRowHeight = 20;
//static const int kColumnWidth = 60;

typedef struct intVector2 {
	int x;
	int y;
}intVector2;

typedef struct Vector2 {
	float x;
	float y;
}Vector2;

typedef struct Vector3{
	float x;
	float y;
	float z;

	Vector3& operator*=(float s) { x *= s;  y *= s; z *= s; return *this; }
	Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator/=(float s) { x /= s;  y /= s; z /= s; return *this; }

}Vector3;

typedef struct Vector4 {
	float x;
	float y;
	float z;
	float w;
}Vector4;

typedef struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
}VertexData;



typedef struct Matrix3x3
{
	float m[3][3];
} Matrix3x3;

typedef struct Matrix4x4
{
	float m[4][4];
} Matrix4x4;

struct AABB {
	Vector3 min;
	Vector3 max;
};

typedef struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
}Material;

typedef struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
}TransformationMatrix;

typedef struct DirectionalLight {
	Vector4 color;	//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
}DirectionalLight;

typedef struct Ball
{

	Vector2 pos;
	Vector2 velocity;
	Vector2 acceleration;
	float mass;
	float radius;
	unsigned int color;

} Ball;

typedef struct Box
{
	Vector2 pos;
	Vector2 size;
	Vector2 valocity;
	Vector2 acceleration;
	float mass;
	unsigned int color;
} Box;

struct Transforms
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct MaterialData
{
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Particle {
	Transforms transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct Emitter {
	Transforms transform; //!< エミッタのTransform
	uint32_t count;	//!< 発生数
	float frequency; //!<　発生頻度
	float frequencyTime; //!<頻度用時刻
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct AccelerationField {
	Vector3 accerelation;	//加速度
	AABB area;	//範囲
};