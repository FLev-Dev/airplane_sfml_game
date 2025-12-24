#include "Enemy.hpp"

class Big_stone final : public Enemy
{
public:

    /** @brief Constructs a big stone enemy with 2 HP, scaled down. */
    Big_stone(sf::Vector2f start_pos) : Enemy("assets/big_stone.png", 2u, start_pos, { -250, 400 })
    {
        score_value_ = 2;
        sprite_->setScale({ 0.2, 0.2 });
    }//!Big_stone

	/** @brief A tightened bounding box for collision detection. */
    FLEV_NODISCARD sf::FloatRect get_bounds() const override
    {
        auto bounds = sprite_->getGlobalBounds();
        float marginX = bounds.size.x;
        float marginY = bounds.size.y * 0.9;
        return sf::FloatRect(
            { bounds.position.x + marginX, bounds.position.y + marginY },
            { bounds.size.x - marginX * 2, bounds.size.y - marginY * 2 }
        );
    }//!get_bounds
};