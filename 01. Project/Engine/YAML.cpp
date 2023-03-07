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