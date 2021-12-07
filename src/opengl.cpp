#include "glad/include/glad/glad.h"

#include "opengl.h"



namespace XGK
{
	namespace OPENGL
	{
		void Uniform::uniformMatrix4fv (Uniform* uniform)
		{
			glUniformMatrix4fv(uniform->location, 1, false, (float*) uniform->wrapper->object_addr);
		}

		Uniform::Uniform (Renderer* _renderer, API::Uniform* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}



		UniformBlock::UniformBlock (Renderer* _renderer, API::UniformBlock* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;



			for (size_t i {}; i < wrapper->uniforms.size(); ++i)
			{
				API::Uniform* uniform_wrapper { wrapper->uniforms[i] };

				Uniform* uniform { new Uniform { renderer, uniform_wrapper } };

				buffer_length += uniform_wrapper->size;

				uniforms.push_back(uniform);
			}



			glCreateBuffers(1, &buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, wrapper->binding, buffer);
			glBufferData(GL_UNIFORM_BUFFER, buffer_length, nullptr, GL_DYNAMIC_DRAW);



			// Initially update uniforms.
			use();
		}

		void UniformBlock::use (void)
		{
			for (size_t i {}; i < uniforms.size(); ++i)
			{
				Uniform* uniform = uniforms[i];

				glBufferSubData(GL_UNIFORM_BUFFER, uniform->wrapper->block_index, uniform->wrapper->size, uniform->wrapper->object_addr);
			}
		}



		Material* Material::used_instance {};

		Material::Material (Renderer* _renderer, API::Material* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;



			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, (const GLchar* const*) wrapper->glsl4_code_vertex.c_str(), nullptr);
			glCompileShader(vertex_shader);

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, (const GLchar* const*) wrapper->glsl4_code_fragment.c_str(), nullptr);
			glCompileShader(fragment_shader);

			program = glCreateProgram();
			glAttachShader(program, vertex_shader);
			glAttachShader(program, fragment_shader);

			glLinkProgram(program);



			// glUseProgram(program);

			// wrapper->uniforms.size() may not be cached since size() is inline method (?)
			for (size_t i {}; i < wrapper->uniforms.size(); ++i)
			{
				API::Uniform* uniform_wrapper { wrapper->uniforms[i] };

				Uniform* uniform { new Uniform { renderer, uniform_wrapper } };

				uniform->location = glGetUniformLocation(program, (const GLchar*) uniform_wrapper->name.c_str());

				if (uniform->location > -1)
				{
					uniform->update = Uniform::uniformMatrix4fv;

					// uniform->update(uniform);

					uniforms.push_back(uniform);
				}
			}

			// glUseProgram(0);



			for (size_t i {}; i < wrapper->uniform_blocks.size(); ++i)
			{
				API::UniformBlock* uniform_block_wrapper { wrapper->uniform_blocks[i] };

				UniformBlock* uniform_block { new UniformBlock { renderer, uniform_block_wrapper } };

				glUniformBlockBinding
				(
					program,
					glGetUniformBlockIndex(program, (const GLchar*) uniform_block_wrapper->name.c_str()),
					uniform_block_wrapper->binding
				);

				uniform_blocks.push_back(uniform_block);
			}



			// Initially update uniforms.
			use();
		}

		void Material::use (void)
		{
			Material::used_instance = this;

			glUseProgram(program);

			for (size_t i {}; i < uniforms.size(); ++i)
			{
				Uniform* uniform = uniforms[i];

				uniform->update(uniform);
			}
		}



		Object::Object (Renderer* _renderer, API::Object* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}

		void Object::draw (void) const
		{
			glDrawArrays
			(
				Material::used_instance->topology,
				wrapper->scene_vertex_data_offset,
				wrapper->scene_vertex_data_length
			);
		}



		Scene::Scene (Renderer* _renderer, API::Scene* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}



		Renderer::Renderer ()
		{
			glfwInit();

			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

			window = glfwCreateWindow(800, 600, "", nullptr, nullptr);

			// glfwSetKeyCallback(window, glfw_key_callback);
			glfwMakeContextCurrent(window);
			glfwSwapInterval(0);
			// glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);



			gladLoadGL();
		}
	}
}
