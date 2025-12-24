#include "Player.hpp"

Player::Player() : Unit("assets/player.png", 3u)
{
    sprite_->setScale({ 0.2f, 0.2f });
}//!Player
//---------------------------------------------------------------------------------------

void Player::update(const float dt)
{
    assert(false && "Player::update(dt) called without screen_size! Use update(dt, screen_size) instead.");

    // Fallback for releases builds
    update(dt, { 800u, 600u });
}//!update
//---------------------------------------------------------------------------------------

void Player::update(const float dt, const sf::Vector2u& screen_size)
{
    const float max_x = static_cast<float>(screen_size.x) / 4.f;
    const float max_y = static_cast<float>(screen_size.y);

    // Arrow keys or WASD
    auto pos = get_position();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        pos.y -= speed_ * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        pos.y += speed_ * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        pos.x -= speed_ * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        pos.x += speed_ * dt;
    }

    // Clamp position within screen bounds
    pos.x = std::clamp(pos.x, 50.f, max_x - 50.f);
    pos.y = std::clamp(pos.y, 50.f, max_y - 50.f);

    set_position(pos);

    // Shooting
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) &&
        shoot_clock_.getElapsedTime().asSeconds() > attack_cooldown_)
    {
        need_to_shoot_ = true;
        shoot_clock_.restart();
    }
    else
    {
        need_to_shoot_ = false;
    }
}//!update
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Player::is_need_to_shoot()
{
    return need_to_shoot_.exchange(false);
}//!is_need_to_shoot
//--------------------------------------------------------------------------------------

FLEV_NODISCARD bool Player::is_out_of_bounds(const sf::Vector2u&) const
{
    return false;
}//!is_out_of_bounds
//--------------------------------------------------------------------------------------

FLEV_NODISCARD float Player::get_speed() const
{ 
    return speed_; 
}//!get_speed
//--------------------------------------------------------------------------------------

FLEV_NODISCARD sf::FloatRect Player::get_bounds() const
{
    const auto bounds = sprite_->getGlobalBounds();
    float margin_x = bounds.size.x * 0.1f;
    float margin_y = bounds.size.y * 0.25f;
    return sf::FloatRect(
        { bounds.position.x + margin_x, bounds.position.y + margin_y * 1.5f },
        { bounds.size.x - margin_x * 2, bounds.size.y - margin_y * 2 }
    );
}//!get_bounds
//--------------------------------------------------------------------------------------