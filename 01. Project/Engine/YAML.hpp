#pragma once
#include "pch.h"

namespace YAML {
	template<>
	struct convert<Vec2> {
		static Node encode(const Vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, Vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vec3> {
		static Node encode(const Vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vec4> {
		static Node encode(const Vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<tTile> {
		static Node encode(const tTile& rhs) {
			Node node;
			node.push_back(rhs.vLeftTop);
			node.push_back(rhs.vSlice);
			return node;
		}

		static bool decode(const Node& node, tTile& rhs) {
			if (!node.IsSequence()) {
				return false;
			}

			rhs.vLeftTop = node[0].as<Vec2>();
			rhs.vSlice = node[1].as<Vec2>();
			return true;
		}
	};

	template<>
	struct convert<tLayerCollision> {
		static Node encode(const tLayerCollision& rhs) {
			Node node;
			node.push_back(rhs.iLeft);
			node.push_back(rhs.iRight);
			return node;
		}

		static bool decode(const Node& node, tLayerCollision& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}

			rhs.iLeft = node[0].as<int>();
			rhs.iRight = node[1].as<int>();
			return true;
		}
	};
}