#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

namespace vr
{
	/// Class that can render 2D text to the screen
	class Text
	{
	public:

		Text(const std::string& text="", const glm::vec2& position = glm::vec2(0, 0), const glm::vec4& color = glm::vec4(1, 1, 0, 1), float scale = 1.0f);
		
		/// Set the text will be rendered
		void setText(const std::string& text);

		/// \return the text string
		const std::string& getText() const;
		
		/// Render the text given a window size
		void render(int width, int height);

		/// Set the color of the text
		void setColor(const glm::vec4& color);

		/// Set the text position in 2D.
		void setPosition(const glm::vec2& position);
		
		/// Set the scale of the text
		void setScale(float scale);

		~Text();

	private:

		static std::string getFontPath(const std::string& fontName = "arial.ttf");

		std::string m_text;
		glm::vec4 m_color;
		glm::vec2 m_position;
		float m_scale;
	};


}