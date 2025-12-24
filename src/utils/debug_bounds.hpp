#pragma	once
#include <utils/defines.hpp>
#include <SFML/Graphics.hpp>

FLEV_DEBUG_NAMESPACE_BEGIN

/** 
 * @brief Draws debug bounds rectangle on the given render target.
 * 
 * In release builds (NDEBUG defined), this function does nothing.
 * 
 * @param render_target[in][out] - The render target to draw on.
 * @param bounds[in]             - The bounding rectangle to visualize.
 */
inline void draw_debug_bounds(sf::RenderTarget& render_target, const sf::FloatRect& bounds)
{
#ifdef NDEBUG
	(void)render_target;
	(void)bounds;
	return;
#else
	sf::RectangleShape rect(bounds.size);
	rect.setPosition(bounds.position);
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::Red);
	rect.setOutlineThickness(1);
	render_target.draw(rect);
#endif //NDEBUG
}//!draw_debug_bounds

FLEV_DEBUG_NAMESPACE_END