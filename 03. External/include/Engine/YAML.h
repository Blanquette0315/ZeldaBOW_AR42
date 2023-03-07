// ==================
// =      YAML      =
// ==================
YAML::Emitter& operator << (YAML::Emitter& out, const Vec2& v);

YAML::Emitter& operator << (YAML::Emitter& out, const Vec3& v);

YAML::Emitter& operator << (YAML::Emitter& out, const Vec4& v);

YAML::Emitter& operator << (YAML::Emitter& out, const tLayerCollision t);

YAML::Emitter& operator << (YAML::Emitter& out, const tTile t);

#include "YAML.hpp"