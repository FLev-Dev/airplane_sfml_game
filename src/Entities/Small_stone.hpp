#include "Enemy.hpp"

class Small_stone final : public Enemy
{
public:

	/** @brief Constructs a small stone enemy with 1 HP, scaled down. */
	Small_stone(sf::Vector2f start_pos): Enemy("assets/small_stone.png", 1u, start_pos, {-450, 200})
	{
		sprite_->setScale({ 0.5, 0.5 });
	}//!Small_stone

	/** @brief A tightened bounding box for collision detection. */
	FLEV_NODISCARD sf::FloatRect get_bounds() const override
	{
		auto bounds = sprite_->getGlobalBounds();
		float marginX = bounds.size.x * 0.35f;
		float marginY = bounds.size.y * 0.4f;
		return sf::FloatRect(
			{ bounds.position.x + marginX, bounds.position.y + marginY },
			{ bounds.size.x - marginX * 2, bounds.size.y - marginY * 2 }
		);
	}//!get_bounds
};