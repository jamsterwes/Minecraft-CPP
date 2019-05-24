#include "shadow_cam.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace lighting
{
    ShadowCam::ShadowCam() : shadowMapResolution(1024)
    {
        proj = glm::ortho(-10.0, 10.0, -10.0, 10.0, 0.01, 1000.0);
        glGenFramebuffers(1, &shadowFramebuffer);
        glGenTextures(1, &shadowMap);
        GenerateBuffers();
    }

    glm::mat4 ShadowCam::GetProj()
    {
        return proj;
    }
    
    glm::mat4 ShadowCam::GetView(glm::vec3 LightDir)
    {
        return glm::lookAt(glm::normalize(LightDir) * glm::vec3(-10.0), LightDir, glm::vec3(0.0, 1.0, 0.0));
    }

    glm::mat4 ShadowCam::GetLightSpace(glm::vec3 LightDir)
    {
        return GetProj() * GetView(LightDir);
    }

    void ShadowCam::GenerateBuffers()
    {
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }
}