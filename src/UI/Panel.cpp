// UI/Panel.cpp
#include "Panel.hpp"

Panel::Panel(const sf::Vector2f& size, const sf::Color fill_color)
{
    background_ = std::make_unique<sf::RectangleShape>(size);
    background_->setFillColor(fill_color);
}//!Panel
//---------------------------------------------------------------------------------------

void Panel::set_size(const sf::Vector2f& size)
{
    if (background_) background_->setSize(size);
}//!set_size
//---------------------------------------------------------------------------------------

void Panel::set_position(const sf::Vector2f& position)
{
    if (background_) background_->setPosition(position);
}//!set_position
//---------------------------------------------------------------------------------------

void Panel::set_fill_color(sf::Color color)
{
    if (background_) background_->setFillColor(color);
}//!set_fill_color
//---------------------------------------------------------------------------------------

void Panel::draw(sf::RenderTarget& render_target) const
{
    if (background_) render_target.draw(*background_);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::FloatRect Panel::get_bounds() const
{
    return background_ ? background_->getGlobalBounds() : sf::FloatRect();
}//!get_bounds
//---------------------------------------------------------------------------------------