#include "pch.h"
#include "YAML.h"

YAML::Emitter& operator << (YAML::Emitter& out, const Vec2& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const Vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const Vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const tLayerCollision t) {
	out << YAML::Flow;
	out << YAML::BeginSeq << t.iLeft << t.iRight << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const tTile t) {
	out << YAML::Flow;
	out << YAML::BeginSeq << t.vLeftTop << t.vSlice << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator << (YAML::Emitter& out, const Matrix t) {
	out << YAML::Flow;
	out << YAML::BeginSeq << t._11 << t._12 << t._13 << t._14 <<
							t._21 << t._22 << t._23 << t._24 <<
							t._31 << t._32 << t._33 << t._34 <<
							t._41 << t._42 << t._43 << t._44 << YAML::EndSeq;
	return out;
}
