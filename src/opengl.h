#ifndef __XGK_API_OPENGL_RENDERER__
#define __XGK_API_OPENGL_RENDERER__



// include <vector>
// #include <cstdint>
// #include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"



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
