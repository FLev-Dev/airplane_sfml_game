#pragma once
#include "Panel.hpp"

/** @brief Panel with background texture and optional title. */
class Decorated_panel : public Panel
{
public:

    /**
	 * @brief Constructs decorated panel with given texture and size.
     * 
	 * @param texture_path[in] - Path to the background texture.
	 * @param size[in][opt]    - Size of the panel. [Default: (0, 0)].
     */
    Decorated_panel(const std::string& texture_path, const sf::Vector2f& size = {0.f, 0.f});

	/** @brief Sets the size of the panel and rescales content area accordingly. */
    void set_size(const sf::Vector2f& size) override;

	/** @brief Sets the position of the panel and repositions content area accordingly. */
    void set_position(const sf::Vector2f& position) override;

	/** @brief Sets the title text displayed at the top of the panel. */
    void set_title(const std::string& title, const sf::Font& font, uint32_t char_size = 24u);

	/** @brief Draws the decorated panel onto the given render target. */
    void draw(sf::RenderTarget& render_target) const override;

    /** @brief Returns inner content area (below header, if any). */
    FLEV_NODISCARD sf::FloatRect get_content_bounds() const;

private:

	std::unique_ptr<sf::Sprite> background_sprite_; ///< Background sprite.
	std::unique_ptr<sf::Text> title_text_;          ///< Title text.

	sf::FloatRect current_content_bounds_;          ///< Current content area bounds.
	sf::Vector2f current_title_center_;             ///< Current title center position.

	// Default pixel values from texture 
	static constexpr float title_y_px_ = 55.f;
    static constexpr sf::FloatRect content_rect_px_{ { 70.f, 100.f}, {237.f, 255.f } };
};