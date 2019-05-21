#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.hpp"

namespace gfx
{
	struct camera
	{
		glm::mat4 view;
		float fov;

		glm::vec2 lookAxis;

		camera(float fov) : fov(fov), view(1.0f), camPos(0.0f), lookAxis(0.0f) { }
		glm::mat4 recalculate(gfx::shader* shader, bool translate = true);
		float getAspectRatio();
		void setView(glm::mat4 newView);

		// DONT SET MANUALLY
		glm::vec3 camPos;
	};
}