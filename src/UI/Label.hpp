#pragma once
#include <utils/defines.hpp>
#include <SFML/Graphics.hpp>
#include <string>

/** @brief Simple text label with position, color and font. */
class Label
{
public:
    /** @brief Constructs empty label. Font must be set before use. */
    Label() = default;

    /** 
     * @brief Constructs label with initial text and font. 
     * 
	 * @param text[in] - Initial text to display.
	 * @param font[in] - Font to use.
	 * @param char_size[in] - Character size in pixels [Default: 24]
     */
    Label(const std::string& text, const sf::Font& font, uint32_t char_size = 24u);

    /** @brief Sets the displayed text (supports std::to_string, etc.). */
    void set_text(const std::string& text);

    /** @brief Sets position (top-left corner). */
    void set_position(const sf::Vector2f& position);

    /** @brief Sets text color. */
    void set_color(const sf::Color color);

    /** @brief Sets character size. */
    void set_char_size(const uint32_t size);

    /** @brief Binds font (required before drawing). */
    void set_font(const sf::Font& font);

    /** @brief Draws the label to the render target. */
    void draw(sf::RenderTarget& render_target) const;

    /** @brief Returns current text bounds. */
    FLEV_NODISCARD sf::FloatRect get_bounds() const;

private:
	std::unique_ptr<sf::Text> text_; ///< SFML Text object
};