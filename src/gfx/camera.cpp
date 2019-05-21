#include "camera.hpp"
#include "util.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace gfx
{
	glm::mat4 camera::recalculate(gfx::shader* shader, bool translate)
	{
		glm::mat4 projection = glm::perspective(glm::radians(fov), getAspectRatio(), 0.1f, 1000.0f);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 dir = glm::vec3(0.0f);
		float radPitch = glm::radians(lookAxis.y);
		float radYaw = glm::radians(lookAxis.x);
		dir.x = cos(radPitch) * cos(radYaw);
		dir.y = sin(radPitch);
		dir.z = cos(radPitch) * sin(radYaw);
		setView(glm::lookAt(camPos, camPos + dir, up));

		if (translate) shader->setMatrix("view", view);
		else shader->setMatrix("view", glm::mat4(glm::mat3(view)));
		shader->setMatrix("proj", projection);
		shader->setVector("CameraPos", camPos);
		return projection;
	}

	void camera::setView(glm::mat4 newView)
	{
		view = newView;
	}

	float camera::getAspectRatio()
	{
		glm::vec2 res = util::getResolution();
		return res.x / res.y;
	}
}