#include "glad/include/glad/glad.h"

#include "opengl.h"



namespace XGK
{
	namespace API
	{
		Object::Object (OpenGLRenderer* _renderer, XGK::API::Object* _wrapper)
		{
			renderer = _renderer;
			wrapper =_ wrapper;
		};

		void Object::draw (void)
		{
			glDrawArrays
			(
				Material::used_instance->topology,
				wrapper->scene_vertex_data_offset,
				wrapper->scene_vertex_data_length
			);
		}



		Scene::Scene (OpenGLRenderer* _renderer, XGK::API::Scene* _wrapper)
		{
			renderer = _renderer;
			wrapper =_ wrapper;
		};



		OpenGLRenderer::OpenGLRenderer ()
		{
			glfwInit();

			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

			window = glfwCreateWindow(800, 600, "", nullptr, nullptr);

			glfwSetKeyCallback(window, glfw_key_callback);
			glfwMakeContextCurrent(window);
			glfwSwapInterval(0);
			// glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);



			gladLoadGL();
		}
	}
}
