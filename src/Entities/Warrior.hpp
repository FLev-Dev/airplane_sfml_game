#include "Enemy.hpp"

class Warrior final : public Enemy
{
public:

    /** @brief Constructs a shooting warrior enemy with 2 HP. */
    Warrior(const sf::Vector2f& start_pos) 
        : Enemy("assets/enemy_warrior.png ", 2u, start_pos, default_velocity_)
    {
        score_value_ = 2;
        sprite_->setScale({ 0.2, 0.2 });
    }//!Warrior

    /** @brief Checks if unit has requested a shot (and consumes the flag). */
    FLEV_NODISCARD bool is_need_to_shoot()
    {
        if (need_to_shoot_) is_shoot_ = true;
        return need_to_shoot_.exchange(false);
    }//!is_need_to_shoot

    /** @brief Required override; forwards to the version with screen_size. */
    void update(const float dt) override
    {
        assert(false && "Warrior::update(dt) called without screen_size! Use update(dt, screen_size) instead.");

        // Fallback for releases builds
        update(dt, { 800u, 600u });
    }//!update

	/** @brief Updates warrior logic (movement, shooting) with screen size. */
    void update(const float dt, const sf::Vector2u& screen_size) override
    {
        auto pos = get_position();

		// If we already shoot - wait sleep time and move to out of screen
        if (is_shoot_)
        {
            if (std::abs(velocity_.x) > std::numeric_limits<float>::epsilon())
            {
                pos += velocity_ * dt;
            }
            else
            {
                sleep_timer_ += dt;
                if (sleep_timer_ >= sleep_time_)
                {
                    velocity_ = default_velocity_; // Resume moving left
                    velocity_.y = 150.f; // Start moving vertically
                    if (pos.y < screen_size.y / 2.f) velocity_.y = -velocity_.y;
                    sleep_timer_ = 0.f;
                }
            }
        }
        else // Move left until 80% of screen width, then stop
        {
            if (std::abs(velocity_.x) > std::numeric_limits<float>::epsilon())
            {
                if (pos.x < screen_size.x * 0.8)
                {
                    velocity_.x = 0.f;
                }
                else
                {
					pos += velocity_ * dt;
                }
            }
            else
            {
				sleep_timer_ += dt;
                if (sleep_timer_ >= sleep_time_)
                {
                    need_to_shoot_ = true;
					sleep_timer_ = 0.f;
                }
            }
        }

        set_position(pos);
    }//!update

private:

	constexpr static sf::Vector2f default_velocity_ = { -250.f, 0.f }; ///< Default leftward velocity.
	float sleep_timer_ = 0.f;   ///< Timer for sleep before shooting.
	float sleep_time_ = 1.f;    ///< Time to wait before and after shooting.

	bool is_shoot_ = false;     ///< Flag indicating if the warrior has shot.
	std::atomic_bool need_to_shoot_ = false; ///< Flag indicating if the warrior needs to shoot.
};