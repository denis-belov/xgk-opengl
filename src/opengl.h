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
		struct RendererBase
		{
			size_t window_width {};
			size_t window_height {};

			GLFWwindow* window {};



			RendererBase (const size_t&, const size_t&);



			virtual void endLoop (void) = 0;
		};



		struct Renderer : public RendererBase
		{
			Renderer (const size_t&, const size_t&);



			virtual void endLoop (void) override;
		};



		struct RendererOffscreen : public RendererBase
		{
			GLuint framebuffer {};
			GLuint framebuffer_renderbuffer_color {};
			GLuint framebuffer_renderbuffer_depth {};
			GLuint pixel_pack_buffer {};

			void* pixel_data {};

			size_t pbo_index {};
			size_t next_pbo_index { 1 };



			RendererOffscreen (const size_t&, const size_t&);



			virtual void endLoop (void) override;
		};



		struct Uniform
		{
			// uniform_update_t functions
			static void uniformMatrix4fv (Uniform*);



			RendererBase* renderer {};
			API::Uniform* wrapper {};

			GLint location {};

			GLint locaiton {};

			using uniform_update_t = void (*) (Uniform*);

			uniform_update_t update {};



			// Isn't Renderer* parameter needed?
			Uniform (RendererBase*, API::Uniform*);
		};



		struct UniformBlock
		{
			RendererBase* renderer {};
			API::UniformBlock* wrapper {};

			GLuint buffer {};

			size_t buffer_length {};

			std::vector<Uniform*> uniforms {};



			UniformBlock (RendererBase*, API::UniformBlock*);



			void use (void);
		};



		struct Material
		{
			static Material* used_instance;



			RendererBase* renderer {};
			API::Material* wrapper {};

			GLenum topology {};

			GLuint program {};

			std::vector<Uniform*> uniforms {};
			std::vector<UniformBlock*> uniform_blocks {};



			Material (RendererBase*, API::Material*);



			void use (void);
		};



		struct Object
		{
			RendererBase* renderer {};
			API::Object* wrapper {};



			Object (RendererBase*, API::Object*);



			void draw (void) const;
		};



		struct Scene
		{
			RendererBase* renderer {};
			API::Scene* wrapper {};



			Scene (RendererBase*, API::Scene*);
		};
	}
}



#endif
