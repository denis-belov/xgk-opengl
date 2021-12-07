#ifndef __XGK_API_OPENGL__
#define __XGK_API_OPENGL__



// include <vector>
// #include <cstdint>
// #include <string>

#include "glfw/glfw-3.3.5/include/GLFW/glfw3.h"

#include "xgk-api/src/material/material.h"
#include "xgk-api/src/object/object.h"
#include "xgk-api/src/scene/scene.h"



namespace XGK
{
	namespace OPENGL
	{
		struct Renderer
		{
			GLFWwindow* window {};



			Renderer (void);
		};



		struct Uniform
		{
			// uniform_update_t functions
			static void uniformMatrix4fv (Uniform*);



			Renderer* renderer {};
			API::Uniform* wrapper {};

			GLint location {};

			GLint locaiton {};

			using uniform_update_t = void (*) (Uniform*);

			uniform_update_t update {};



			// Isn't Renderer* parameter needed?
			Uniform (Renderer*, API::Uniform*);
		};



		struct UniformBlock
		{
			Renderer* renderer {};
			API::UniformBlock* wrapper {};

			GLuint buffer {};

			size_t buffer_length {};

			std::vector<Uniform*> uniforms {};



			UniformBlock (Renderer*, API::UniformBlock*);



			void use (void);
		};



		struct Material
		{
			static Material* used_instance;



			Renderer* renderer {};
			API::Material* wrapper {};

			GLenum topology {};

			GLuint program {};

			std::vector<Uniform*> uniforms {};
			std::vector<UniformBlock*> uniform_blocks {};



			Material (Renderer*, API::Material*);



			void use (void);
		};



		struct Object
		{
			Renderer* renderer {};
			API::Object* wrapper {};



			Object (Renderer*, API::Object*);



			void draw (void) const;
		};



		struct Scene
		{
			Renderer* renderer {};
			API::Scene* wrapper {};



			Scene (Renderer*, API::Scene*);
		};
	}
}



#endif
