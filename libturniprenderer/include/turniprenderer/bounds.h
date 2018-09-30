#pragma once

#include "private/external/glm.h"
#include <limits>

namespace TurnipRenderer {
	class Bounds {
	public:
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
		
		inline glm::vec3 getCentre() const {
			return (max + min) / 2.0f;
		}
		inline glm::vec3 getExtents() const {
			return (max - min) / 2.0f;
		}

		inline void encapsulate(glm::vec3 point){
			min = glm::min(min, point);
			max = glm::max(max, point);
		}
		inline void encapsulate(Bounds bounds){
			min = glm::min(min, bounds.min);
			max = glm::max(max, bounds.max);
		}
		inline bool contains(glm::vec3 point){
			return point.x >= min.x && point.x <= max.x
				&& point.y >= min.y && point.y <= max.y
				&& point.z >= min.z && point.z <= max.z;
		}
		inline bool contains(Bounds bounds){
			return contains(bounds.min) && contains(bounds.max);
		}

		friend Bounds operator*(glm::mat4, const Bounds&);
	};

	inline Bounds operator*(glm::mat4 transformation, const Bounds& bounds){
		const glm::vec4 corners[8] = {
			{bounds.min.x, bounds.min.y, bounds.min.z, 1},
			{bounds.min.x, bounds.min.y, bounds.max.z, 1},
			{bounds.min.x, bounds.max.y, bounds.min.z, 1},
			{bounds.min.x, bounds.max.y, bounds.max.z, 1},
			{bounds.max.x, bounds.min.y, bounds.min.z, 1},
			{bounds.max.x, bounds.min.y, bounds.max.z, 1},
			{bounds.max.x, bounds.max.y, bounds.min.z, 1},
			{bounds.max.x, bounds.max.y, bounds.max.z, 1},
		};
		Bounds newBounds;
		for (int i = 0; i < 8; i++){
			newBounds.encapsulate(glm::vec3{transformation * corners[i]});
		}
		return newBounds;
	}
	inline Bounds operator*(glm::quat rotation, const Bounds& bounds){
		return glm::mat4_cast(rotation) * bounds;
	}
};
