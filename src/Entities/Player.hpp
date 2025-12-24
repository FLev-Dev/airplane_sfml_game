#pragma once
#include "Unit.hpp"
#include <SFML/Graphics.hpp>

class Player final : public Unit
{
public:

	/** @brief Constructs player with default texture and 3 HP. */
    Player();

    /** @brief Required override; forwards to the version with screen_size. */
    void update(const float dt) override;

	/** @brief Updates player logic (movement, shooting) with screen size. */
    void update(const float dt, const sf::Vector2u& screen_size) override;


    /** @brief Checks if player has requested a shot (and consumes the flag). */
    FLEV_NODISCARD bool is_need_to_shoot();

    /** @brief Player never goes out of bounds by itself (handled by movement clamp). */
    FLEV_NODISCARD bool is_out_of_bounds(const sf::Vector2u&) const override;

	/** @returns The player's movement speed. */
    FLEV_NODISCARD float get_speed() const;

	/** @returns A tightened bounding box for collision detection. */
    FLEV_NODISCARD sf::FloatRect get_bounds() const override;

private:
	sf::Clock shoot_clock_;                  ///< Clock to manage shooting cooldown.
	float speed_ = 300.f;                    ///< Movement speed in pixels per second.
	float attack_cooldown_ = 0.7f;           ///< Time between shots in seconds.
	std::atomic_bool need_to_shoot_ = false; ///< Flag indicating if a shot is requested.
};