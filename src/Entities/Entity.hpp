#pragma once
#include <utils/defines.hpp>
#include <utils/logger.hpp>
#include <SFML/Graphics.hpp>
#include <map>

/** @brief Base class for all in-game entities (player, enemies, etc.). */
class Entity
{
public:
    /** @brief Constructs entity and loads its texture (cached per path). */
    Entity(const std::string& texture_path)
    {
        TODO("Move into future resourse manager")
        static std::map<std::string, sf::Texture> textures;

        if (!textures.contains(texture_path))
        {
            if (!textures[texture_path].loadFromFile(texture_path))
            {
                LOG_ERROR(
                    get_global_logger(),
                    "Failed to load texture from path: {}",
                    texture_path
				);
                textures.erase(texture_path);
            }
        }
        sprite_ = std::make_unique<sf::Sprite>(textures.at(texture_path));
        sprite_->setOrigin(sprite_->getLocalBounds().getCenter());
    }//!Entity

    virtual ~Entity() = default;

    /** @brief Updates entity logic (to be implemented by derived classes). */
    virtual void update(const float dt) = 0;
    virtual void update(const float dt, const sf::Vector2u& screen_size) { update(dt); };

    /** @brief Draws the entity sprite to the given target. */
    virtual void draw(sf::RenderTarget& render_target) const { render_target.draw(*sprite_); }

    /** @returns The global bounding rectangle of the entity. */
    virtual FLEV_NODISCARD sf::FloatRect get_bounds() const { return sprite_->getGlobalBounds(); }

    /** @returns Current position of the entity. */
    virtual FLEV_NODISCARD sf::Vector2f get_position() const { return sprite_->getPosition(); }

    /** @brief Sets the entity's position. */
    virtual void set_position(const sf::Vector2f& position) { sprite_->setPosition(position); }

    /** @brief Checks if the entity is outside the screen bounds. */
    virtual FLEV_NODISCARD bool is_out_of_bounds(const sf::Vector2u& screen_size) const
    {
        sf::FloatRect bounds = get_bounds();
        return (bounds.position.x < 0 || bounds.position.x > screen_size.x) ||
               (bounds.position.y < 0 || bounds.position.y > screen_size.y);
    }//!is_out_of_bounds

protected:
    std::unique_ptr<sf::Sprite> sprite_; ///< Sprite representing the entity.
};