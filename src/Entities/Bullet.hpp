#pragma once
#include "Entity.hpp"
#include <SFML/Graphics.hpp>

/** @brief Projectile fired by units (e.g. player). */
class Bullet : public Entity
{
public:
    /** @brief Constructs bullet at start position with given velocity. */
    Bullet(
        sf::Vector2f start_pos, 
        sf::Vector2f velocity, 
        const std::string& texture_path = "assets/bullet.png",
        const sf::Vector2f& scale = { 0.2f, 0.2f }
    )
        : Entity(texture_path)
        , velocity_(velocity)
    {
        sprite_->setScale(scale);
        set_position(start_pos);
    }//!Bullet

    /** @brief Updates bullet position based on velocity. */
    void update(const float dt) override
    {
        auto pos = get_position();
        pos += velocity_ * dt;
        set_position(pos);
    }//!update

private:
	sf::Vector2f velocity_; ///< Velocity of the bullet.
};//!class Bullet