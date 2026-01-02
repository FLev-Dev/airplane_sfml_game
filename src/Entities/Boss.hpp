#include "Enemy.hpp"
#include <random>
#include <limits>

class Boss final : public Enemy
{
    enum class Phase
    {
        Appearance,
        Attacking,
        Moving
    };

    enum class Attack_subphase
    {
        Preparing,   ///< Wait before shooting.
        Recovering   ///< Wait after shooting.
    };

    using enum Phase;
    using enum Attack_subphase;

public:

    /** @brief Constructs a shooting boss enemy with 20 HP. */
    Boss(const sf::Vector2f& start_pos)
        : Enemy("assets/enemy_boss.png", 20u, start_pos, default_velocity_)
        , rng_(std::random_device{}())
    {
        score_value_ = 20;
        sprite_->setScale({ 0.6, 0.6 });
        phase_ = Appearance;
        attack_subphase_ = Preparing;
    }//!Boss

    /** @brief Checks if unit has requested a shot (and consumes the flag). */
    FLEV_NODISCARD bool is_need_to_shoot()
    {
        if (need_to_shoot_) is_shoot_ = true;
        return need_to_shoot_.exchange(false);
    }//!is_need_to_shoot

    /** @brief Required override; forwards to the version with screen_size. */
    void update(const float dt) override
    {
        assert(false && "Boss::update(dt) called without screen_size! Use update(dt, screen_size) instead.");

        // Fallback for releases builds
        update(dt, { 800u, 600u });
    }//!update

    /** @brief Updates boss logic (movement, shooting) with screen size. */
    void update(const float dt, const sf::Vector2u& screen_size) override
    {
        auto pos = get_position();

        // Define patrol boundaries: right third horizontally, middle 60% vertically.
        const float right_third_start = screen_size.x * 2.0f / 3.0f;
        const float x_min = right_third_start;
        const float x_max = static_cast<float>(screen_size.x) - screen_size.x * 0.1f;
        const float y_margin = screen_size.y * 0.3f;
        const float y_min = y_margin;
        const float y_max = static_cast<float>(screen_size.y) - y_margin;

        switch (phase_)
        {
        case Appearance:
        {
            if (std::abs(velocity_.x) > std::numeric_limits<float>::epsilon())
            {
                pos += velocity_ * dt;
                if (pos.x <= x_min)
                {
                    pos.x = x_min;
                    sleep_timer_ = 0.f;
                    velocity_ = { 0.f, 0.f };
                    phase_ = Attacking;
                }
            }
            break;
        }
        case Attacking:
        {
            sleep_timer_ += dt;

            if (attack_subphase_ == Preparing)
            {
                if (sleep_timer_ >= sleep_time_)
                {
                    need_to_shoot_ = true; // Trigger shot once after 1 second.
                    sleep_timer_ = 0.f;
                    attack_subphase_ = Recovering;
                }
            }
            else if (attack_subphase_ == Recovering)
            {
                if (sleep_timer_ >= sleep_time_)
                {
                    // Post-shot delay complete; begin patrol movement.
                    patrol_timer_ = 0.f;
                    attack_subphase_ = Preparing;
                    choose_random_velocity();
                    phase_ = Moving;
                }
            }
            break;
        }
        case Moving:
        {
            pos += velocity_ * dt;
            patrol_timer_ += dt;

            // Keep boss within patrol zone (right third, vertical margins).
            if (pos.x < x_min)      { pos.x = x_min; velocity_.x = -velocity_.x; }
            else if (pos.x > x_max) { pos.x = x_max; velocity_.x = -velocity_.x; }
            if (pos.y < y_min)      { pos.y = y_min; velocity_.y = -velocity_.y; }
            else if (pos.y > y_max) { pos.y = y_max; velocity_.y = -velocity_.y; }

            if (patrol_timer_ >= patrol_time_)
            {
                // Patrol time elapsed; stop and prepare next attack cycle.
                velocity_ = { 0.f, 0.f };
                phase_ = Attacking;
                sleep_timer_ = 0.f;
                attack_subphase_ = Preparing;
            }
            break;
        }
        }

        set_position(pos);
    }//!update

private/*methods*/:

    /** @brief Sets a new random velocity within patrol limits. */
    void choose_random_velocity()
    {
        const float speed = speed_dist_(rng_);
        const float angle = angle_dist_(rng_);
        velocity_.x = speed * std::cos(angle);
        velocity_.y = speed * std::sin(angle);
    }//!choose_random_velocity

private/*vars*/:

    constexpr static sf::Vector2f default_velocity_ = { -250.f, 0.f }; ///< Default leftward velocity.
    float sleep_timer_ = 0.f;   ///< Timer for sleep before and after shooting.
    float sleep_time_ = 1.f;    ///< Duration of pre- and post-shot delay.

    float patrol_timer_ = 0.f;  ///< Timer for random patrol movement.
    float patrol_time_ = 1.f;  ///< Duration of patrol movement phase.

    Phase phase_ = Appearance;                    ///< Current phase of the boss behavior.
    Attack_subphase attack_subphase_ = Preparing; ///< Subphase during attack.
    bool is_shoot_ = false;                       ///< Flag indicating if the boss has shot.
    std::atomic_bool need_to_shoot_ = false;      ///< Flag indicating if the boss needs to shoot.

    // Random number generator for patrol direction.
    std::mt19937 rng_;
    std::uniform_real_distribution<float> speed_dist_{ 250.0f, 400.0f };
    std::uniform_real_distribution<float> angle_dist_{ 0.0f, 2.0f * 3.1415926535f };
};