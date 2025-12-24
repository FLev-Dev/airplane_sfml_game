#pragma once
#include <utils/defines.hpp>
#include <SFML/Graphics.hpp>

class Button
{
public:

    /** 
	 * @brief Constructs a button with given text and font.
     * 
	 * @param text[in] - Text to display on the button.
	 * @param font[in] - Font to use for the button text.
	 * @param char_size[in][opt] - Character size for the button text [Default: 30].
     */
    Button(const std::string& text, const sf::Font& font, uint32_t char_size = 30u);

	/**
     * @brief Constructs a button with given rectangle area, text and font.
	 *
	 * @param rect[in] - Rectangle defining button position and size.
	 * @param text[in] - Text to display on the button.
	 * @param font[in] - Font to use for the button text.
	 * @param char_size[in][opt] - Character size for the button text [Default: 30].
     */
    Button(const sf::FloatRect& rect, const std::string& text, const sf::Font& font, uint32_t char_size = 30u);

	/** @brief Sets the position of the button. */
    void set_position(const sf::Vector2f& position);
    
    /** @brief Set the color of the button text. */
    void set_text_color(sf::Color color);

    /** @brief Set the fill color of the button body. */
    void set_fill_color(sf::Color color);

	/** @brief Set the outline color of the button body. */
    void set_outline_color(sf::Color color);

	/** @brief Set the outline thickness of the button body. */
    void set_outline_thickness(const float thickness);

	/** @brief Enable or disable the button. Disabled buttons do not respond to events. */
	void set_enabled(bool enabled);

    /** 
     * @brief Process a single event and update internal state (hover/pressed).
     * @returns true if a click was completed (press + release on button). 
     */
    FLEV_NODISCARD bool handle_event(const sf::Event& event);

	/** @brief Draws the button onto the given target. */
    void draw(sf::RenderTarget& render_target) const;

private/*methods*/:

	/** @brief Update visual appearance based on current state (hovered, pressed). */
    void update_visuals();

private/*vars*/:

	sf::RectangleShape body_;   ///< Button body shape
	sf::Text label_;            ///< Button text label

	bool is_hovered_ = false;   ///< Is the mouse currently hovering over the button
	bool is_pressed_ = false;	///< Is the button currently pressed
	bool is_enabled_ = true;	///< Is the button enabled

	bool clicked_ = false;		///< Has the button been clicked (pressed and released)
};