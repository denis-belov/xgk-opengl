#include "glad/include/glad/glad.h"

#include "xgk-opengl/src/opengl.h"



#include <iostream>
using std::cout;
using std::endl;



namespace XGK
{
	namespace OPENGL
	{
		RendererBase::RendererBase (API::Renderer* _wrapper)
		{
			// cout << (char *) glGetString(GL_VERSION) << endl;
			// cout << (char *) glGetString(GL_VENDOR) << endl;
			// cout << (char *) glGetString(GL_RENDERER) << endl;



			wrapper = _wrapper;



			glfwInit();

			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

			window = glfwCreateWindow(wrapper->width, wrapper->height, "", nullptr, nullptr);

			// glfwSetKeyCallback(window, glfw_key_callback);
			glfwMakeContextCurrent(window);
			// glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);



			gladLoadGL();



			glViewport(0, 0, wrapper->width, wrapper->height);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			// window = glfwCreateWindow(window_width, window_height, "", nullptr, nullptr);
		}



		Renderer::Renderer (API::Renderer* _wrapper) : RendererBase(_wrapper)
		{
			glfwSwapInterval(0);
		}

		void Renderer::endLoop (void)
		{
			glfwSwapBuffers(window);
		}



		RendererOffscreen::RendererOffscreen (API::Renderer* _wrapper) : RendererBase(_wrapper)
		{
			glfwHideWindow(window);

			glfwSwapInterval(1);



			// Framebuffer object for offscreen rendering
			{
				glCreateFramebuffers(1, &framebuffer);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);

				glCreateRenderbuffers(1, &framebuffer_renderbuffer_color);
				glBindRenderbuffer(GL_RENDERBUFFER, framebuffer_renderbuffer_color);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, wrapper->width, wrapper->height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, framebuffer_renderbuffer_color);

				// Depth attachment
				// TODO: make depth buffer optional.
				{
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LEQUAL);

					glCreateRenderbuffers(1, &framebuffer_renderbuffer_depth);
					glBindRenderbuffer(GL_RENDERBUFFER, framebuffer_renderbuffer_depth);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, wrapper->width, wrapper->height);
					glBindRenderbuffer(GL_RENDERBUFFER, 0);

					glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer_renderbuffer_depth);
				}

				// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			}



			// Pixel pack buffer
			{
				glCreateBuffers(1, &pixel_pack_buffer);

				glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer);
				// TODO: cache wrapper->width * wrapper->height * 4
				glBufferData(GL_PIXEL_PACK_BUFFER, wrapper->width * wrapper->height * 4, nullptr, GL_DYNAMIC_READ);

				// Redundant call. GL_COLOR_ATTACHMENT0 is a default framebuffer read buffer.
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glReadPixels(0, 0, wrapper->width, wrapper->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

				pixel_data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			}
		}

		void RendererOffscreen::endLoop (void)
		{
			// glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer);
			glBufferData(GL_PIXEL_PACK_BUFFER, wrapper->width * wrapper->height * 4, nullptr, GL_DYNAMIC_READ);
			// glReadBuffer(GL_COLOR_ATTACHMENT0);
			glReadPixels(0, 0, wrapper->width, wrapper->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

			pixel_data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

			// if (pixel_data)
			// {
			// 	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			// }

			// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			// glDrawBuffer(GL_BACK);
			// glReadBuffer(GL_FRONT);
		}



		void Uniform::uniformMatrix4fv (Uniform* uniform)
		{
			glUniformMatrix4fv(uniform->location, 1, false, (float*) uniform->wrapper->object_addr);
		}

		Uniform::Uniform (RendererBase* _renderer, API::Uniform* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}



		UniformBlock::UniformBlock (RendererBase* _renderer, API::UniformBlock* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;



			for (API::Uniform* uniform_wrapper : wrapper->uniforms)
			{
				Uniform* uniform { new Uniform { renderer, uniform_wrapper } };

				buffer_length += uniform_wrapper->size;

				uniforms.push_back(uniform);
			}



			glGenBuffers(1, &buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, wrapper->binding, buffer);
			glBufferData(GL_UNIFORM_BUFFER, buffer_length, nullptr, GL_DYNAMIC_DRAW);



			// Initially update uniforms.
			use();
		}

		void UniformBlock::use (void)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);

			// for (size_t i {}; i < uniforms.size(); ++i)
			// {
			// 	Uniform* uniform = uniforms[i];

			// 	glBufferSubData(GL_UNIFORM_BUFFER, uniform->wrapper->block_index, uniform->wrapper->size, uniform->wrapper->object_addr);
			// }

			for (Uniform* uniform : uniforms)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, uniform->wrapper->block_index, uniform->wrapper->size, uniform->wrapper->object_addr);
			}
		}



		Material* Material::used_instance {};

		Material::Material (RendererBase* _renderer, API::Material* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;



			program = glCreateProgram();



			const GLchar* _glsl4_code_vertex { wrapper->glsl4_code_vertex.c_str() };

			GLuint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(shader_vertex, 1, &_glsl4_code_vertex, nullptr);
			glCompileShader(shader_vertex);

			{
				GLint isCompiled = 0;
				glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &isCompiled);
				if(isCompiled == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetShaderiv(shader_vertex, GL_INFO_LOG_LENGTH, &maxLength);

					// The maxLength includes the NULL character
					std::vector<GLchar> errorLog(maxLength);
					glGetShaderInfoLog(shader_vertex, maxLength, &maxLength, &errorLog[0]);

					for (GLchar err : errorLog)
					{
						cout << err;
					}

					cout << endl;

					// // Provide the infolog in whatever manor you deem best.
					// // Exit with failure.
					// glDeleteShader(shader_vertex); // Don't leak the shader.
					// return;
				}
			}

			glAttachShader(program, shader_vertex);



			const GLchar* _glsl4_code_fragment { wrapper->glsl4_code_fragment.c_str() };

			GLuint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(shader_fragment, 1, &_glsl4_code_fragment, nullptr);
			glCompileShader(shader_fragment);

			{
				GLint isCompiled = 0;
				glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &isCompiled);
				if(isCompiled == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetShaderiv(shader_fragment, GL_INFO_LOG_LENGTH, &maxLength);

					// The maxLength includes the NULL character
					std::vector<GLchar> errorLog(maxLength);
					glGetShaderInfoLog(shader_fragment, maxLength, &maxLength, &errorLog[0]);

					for (GLchar err : errorLog)
					{
						cout << err;
					}

					cout << endl;

					// // Provide the infolog in whatever manor you deem best.
					// // Exit with failure.
					// glDeleteShader(shader_fragment); // Don't leak the shader.
					// return;
				}
			}

			glAttachShader(program, shader_fragment);



			glLinkProgram(program);



			for (API::Uniform* uniform_wrapper : wrapper->uniforms)
			{
				Uniform* uniform { new Uniform { renderer, uniform_wrapper } };

				uniform->location = glGetUniformLocation(program, (const GLchar*) uniform_wrapper->name.c_str());

				if (uniform->location > -1)
				{
					uniform->update = Uniform::uniformMatrix4fv;

					uniforms.push_back(uniform);
				}
			}



			for (API::UniformBlock* uniform_block_wrapper : wrapper->uniform_blocks)
			{
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

			// for (size_t i {}; i < uniforms.size(); ++i)
			// {
			// 	Uniform* uniform = uniforms[i];

			// 	uniform->update(uniform);
			// }

			for (Uniform* uniform : uniforms)
			{
				uniform->update(uniform);
			}
		}



		Object::Object (RendererBase* _renderer, API::Object* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}

		void Object::draw (void) const
		{
			glDrawArrays
			(
				// Material::used_instance->topology,
				GL_TRIANGLES,
				wrapper->scene_vertex_data_offset,
				wrapper->scene_vertex_data_length
			);
		}



		Scene::Scene (RendererBase* _renderer, API::Scene* _wrapper)
		{
			renderer = _renderer;
			wrapper = _wrapper;
		}
	}
}
