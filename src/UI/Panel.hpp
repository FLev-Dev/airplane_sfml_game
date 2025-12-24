#pragma once
#include <utils/defines.hpp>
#include <utils/logger.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

/** @brief Basic semi-transparent background panel (no texture). */
class Panel
{
public:

    /**
     * @brief Constructs panel with given size and fill color.
     *
     * @param size[in][opt]       - Size of the panel. [Default: (0, 0)].
     * @param fill_color[in][opt] - Fill color of the panel. [Default: semi-transparent dark color].
	 */
    Panel(const sf::Vector2f& size = {0.f, 0.f}, const sf::Color fill_color = sf::Color(30, 30, 40, 220));
    virtual ~Panel() = default;

    /** @brief Sets the size of the panel. */
    virtual void set_size(const sf::Vector2f& size);

    /** @brief Sets the position of the panel. */
    virtual void set_position(const sf::Vector2f& position);
    
	/** @brief Sets the fill color of the panel. */
    void set_fill_color(sf::Color color);

	/** @brief Draws the panel onto the given render target. */
    virtual void draw(sf::RenderTarget& render_target) const;

	/** @brief Gets the bounds of the panel. */
    FLEV_NODISCARD sf::FloatRect get_bounds() const;

protected:

	std::unique_ptr<sf::RectangleShape> background_; ///< Background rectangle shape.
};