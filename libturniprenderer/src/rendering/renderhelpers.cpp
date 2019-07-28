#include "turniprenderer/rendering/renderhelpers.h"

#include "turniprenderer/context.h"

namespace TurnipRenderer::RenderHelpers {
	void drawEntity(Context& context,
					Entity& entity,
					StaticRenderData& renderData) {
		glm::mat4 M = entity.transform.transformWorldSpaceFromModelSpace();
		glm::mat4 MVP = renderData.transformProjectionFromWorld * M;
		
		if (entity.shader){
			glUseProgram(entity.shader->programId);
			
			context.renderer.bindMaterial(entity.material);
			
			glm::mat4 lightMVP;
			glm::vec3 lightDirection;
			if (renderData.shadowmap){
				context.renderer.bindTextureToSlot(GL_TEXTURE1, renderData.shadowmap->colorBuffer);
				glUniform1i(3, 1);
				context.renderer.bindTextureToSlot(GL_TEXTURE2, renderData.shadowmap->depthBuffer);
				glUniform1i(4, 2);
				lightMVP = renderData.shadowmap->VP * M;
				lightDirection = glm::inverse(renderData.shadowmap->V) * glm::vec4(0,0,1,0);
			}

			glUniformMatrix4fv(1, 1, GL_FALSE,
							   reinterpret_cast<const GLfloat*>(&M));
			glUniformMatrix4fv(2, 1, GL_FALSE,
							   reinterpret_cast<const GLfloat*>(&lightMVP));
			glUniform3fv(5, 1,
						 reinterpret_cast<const GLfloat*>(&renderData.cameraPos));
			glUniform3fv(6, 1,
						 reinterpret_cast<const GLfloat*>(&lightDirection));
		}else{
			glUseProgram(context.getDebugShaders().debugOpaqueShader->programId);
		}
		
		glUniformMatrix4fv(0, 1, GL_FALSE,
						   reinterpret_cast<const GLfloat*>(&MVP));
		
		context.renderer.drawMesh(*entity.mesh);
	}
}
