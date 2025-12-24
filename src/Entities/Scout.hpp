#include "Enemy.hpp"

class Scout final : public Enemy
{
public:

	/** @brief Constructs a small stone enemy with 1 HP, scaled down. */
    Scout(const sf::Vector2f& start_pos) : Enemy("assets/enemy_scout.png ", 2u, start_pos, {-400.f, 0.f})
	{
		sprite_->setScale({ 0.2, 0.2 });
	}//!Small_stone

    /** @brief Required override; forwards to the version with screen_size. */
    void update(const float dt) override
    {
        assert(false && "Scout::update(dt) called without screen_size! Use update(dt, screen_size) instead.");	
        
        // Fallback for releases builds
        update(dt, { 800u, 600u });
    }//!update

    /** @brief Updates scout logic (movement) with screen size. */
	void update(const float dt, const sf::Vector2u& screen_size) override
	{
        using enum Direction;
        auto pos = get_position();

        switch (direction_)
        {
        case Right:
        {
            pos.x += velocity_.x * dt;
            break;
        }
        case Left:
        {
            pos.x += velocity_.x * dt;
            if (pos.x <= 300.f)
            {
                direction_ = Turning;
                turn_timer_ = 0.f;
                start_turn_pos_ = pos;
            }
            break;
        }

        case Turning:
        {
            const bool turn_downward_ = (start_turn_pos_.y < screen_size.y / 2.f);
            turn_timer_ += dt;
            const float time = std::min(turn_timer_ / turn_duration_, 1.0f);
            
            const auto angle = 180.f * time;
            sprite_->setRotation(sf::degrees(turn_downward_ ? -angle : angle));

            const auto end_y = turn_downward_
                ? start_turn_pos_.y + turn_radius_ * 2 
                : start_turn_pos_.y - turn_radius_ * 2;
            pos.y = start_turn_pos_.y + (end_y - start_turn_pos_.y) * time;

            const float x_offset = turn_radius_ * std::sin(time * 3.1415926535f); // PI 
            pos.x = start_turn_pos_.x - x_offset;

            if (time >= 1.0f)
            {
				velocity_.x = std::abs(velocity_.x) + 100.f; // Speed boost after turn
                direction_ = Right;
            }
            break;
        }
        default:
        {
			assert(false && "Scout enemy has invalid movement direction!");

            std::call_once(invalid_direction_flag_, []() {
                LOG_ERROR(get_global_logger(), "Scout enemy has invalid movement direction!");
            });
            break;
        }
        }
        set_position(pos);
	}//!update

private:

	static constexpr float turn_duration_ = 1.0f; ///< Duration of the turn maneuver in seconds.
	static constexpr float turn_radius_ = 100.f;  ///< Radius of the turn maneuver.

	std::once_flag invalid_direction_flag_;       ///< Flag to ensure invalid direction logging happens only once.

    enum class Direction
    {
		Left,   ///< Moving leftwards.
		Right,  ///< Moving rightwards.
		Turning ///< Currently performing a turn maneuver.

    } direction_ = Direction::Left;               ///< Current movement direction.
    sf::Vector2f start_turn_pos_{};               ///< Position where the turn started.
	float turn_timer_ = 0.f;                      ///< Timer for the turn maneuver.
};