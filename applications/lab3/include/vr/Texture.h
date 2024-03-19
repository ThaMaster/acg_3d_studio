#pragma once

#include <glad/glad.h>

#include <string>


namespace vr
{
	class Texture
	{
	public:

		/// <summary>
		/// Constructor for an image
		/// </summary>
		/// <param name="image">path to an image on disk</param>
		/// <param name="texType"></param>
		/// <param name="slot">texture slot (default 0)</param>
		/// <param name="format"></param>
		/// <param name="pixelType"></param>
		Texture();
		bool create(const char* image, unsigned int slot=0, GLenum texType = GL_TEXTURE_2D, GLenum pixelType=GL_UNSIGNED_BYTE);
		bool createProceduralTexture(unsigned int slot=0);

		bool isValid();

		~Texture();

		/// Assigns a texture unit to a texture
		void texUnit(GLuint program, const char* uniform, GLuint unit);

		/// Binds a texture
		void bind();

		/// Unbinds a texture
		void unbind();
		
		/// Deletes a texture
		void cleanup();

		bool isProcedural(void) { return m_isProcedural; }

	private:
		GLuint m_id;
		GLenum m_type;
		bool m_valid;
		bool m_isProcedural = false;
		GLuint m_textureSlot;

	};
}


