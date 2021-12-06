#ifndef __XGK_API_OPENGL__
#define __XGK_API_OPENGL__



// include <vector>
// #include <cstdint>
// #include <string>

#include "glfw/glfw-3.3.5/include/GLFW/glfw3.h"

#include "xgk-api/src/object/object.h"
#include "xgk-api/src/scene/scene.h"



namespace XGK
{
	namespace API
	{
		struct OpenGLRenderer;

		struct Uniform
		{};

		struct UniformBlock
		{};

		struct Material
		{};

		struct Object
		{
			OpenGLRenderer* renderer {};
			XGK::API::Object wrapper {};



			Object (OpenGLRenderer*, Object*);



			void draw (void) const;
		};

		struct Scene
		{
			OpenGLRenderer* renderer {};
			XGK::API::Scene wrapper {};



			Scene (OpenGLRenderer*, Scene*);
		};



		struct OpenGLRenderer
		{
			GLFWwindow* window {};



			OpenGLRenderer (void);
		};
	}
}



#endif
