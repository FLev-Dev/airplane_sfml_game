#pragma once
#include "Unit.hpp"
#include <SFML/Graphics.hpp>

class Enemy : public Unit
{
public:
    Enemy(const std::string& texture_path, uint32_t max_hp, sf::Vector2f start_pos, sf::Vector2f velocity)
        : Unit(texture_path, max_hp)
        , velocity_(velocity)
    {
        set_position(start_pos);
    }//!Enemy

    using Unit::update;
    void update(const float dt) override
    {
        auto pos = get_position();
        pos += velocity_ * dt;
        set_position(pos);
    }//!update
     
    /** @brief Checks if the enemy is out of the left screen bounds. */
    FLEV_NODISCARD bool is_out_of_bounds(const sf::Vector2u& screen_size) const override
    {
		const auto bounds = get_bounds();
        return (bounds.position.x + bounds.size.x < 0);
    }//!is_out_of_bounds

    /** @brief Score awarded to player when this enemy is destroyed. */
    FLEV_NODISCARD virtual int32_t get_score_value() const { return score_value_; }

protected:
    
	int32_t score_value_ = 1; ///< Score awarded when destroyed.
	sf::Vector2f velocity_;   ///< Movement velocity.
};