#include "Button.hpp"

Button::Button(const std::string& text, const sf::Font& font, uint32_t char_size)
    : label_(font, sf::String::fromUtf8(text.begin(), text.end()), char_size)
{
    label_.setFillColor(sf::Color::White);

    // Autosize with padding
    sf::FloatRect text_bounds = label_.getLocalBounds();
    float padding_X = 20.f;
    float padding_Y = 10.f;

    body_.setSize(sf::Vector2f(
        text_bounds.size.x + padding_X * 2,
        text_bounds.size.y + padding_Y * 2
    ));
    body_.setFillColor(sf::Color(50, 50, 50, 200));
    body_.setOutlineColor(sf::Color::White);
    body_.setOutlineThickness(2.f);

	// Center the text in the button
    label_.setOrigin(text_bounds.getCenter());
}//!Button
//---------------------------------------------------------------------------------------

Button::Button(const sf::FloatRect& rect, const std::string& text, const sf::Font& font, uint32_t char_size)
    : Button(text, font, char_size)
{
    // Center the text in the button
    body_.setSize(rect.size);
    body_.setPosition(rect.position);
    label_.setPosition(body_.getGlobalBounds().getCenter());
    label_.setOrigin(label_.getLocalBounds().getCenter());
}//!Button
//---------------------------------------------------------------------------------------

void Button::set_position(const sf::Vector2f& position)
{
    body_.setPosition(position);
    label_.setPosition({
        position.x + body_.getSize().x / 2.f,
        position.y + body_.getSize().y / 2.f
    });
}//!set_position
//---------------------------------------------------------------------------------------

void Button::set_text_color(sf::Color color)
{
    label_.setFillColor(color);
}//!set_text_color
//---------------------------------------------------------------------------------------

void Button::set_fill_color(sf::Color color)
{
    body_.setFillColor(color);
}//!set_fill_color
//---------------------------------------------------------------------------------------

void Button::set_outline_color(sf::Color color)
{
    body_.setOutlineColor(color);
}//!set_outline_color
//---------------------------------------------------------------------------------------

void Button::set_outline_thickness(float thickness)
{
    body_.setOutlineThickness(thickness);
}//!set_outline_thickness
//---------------------------------------------------------------------------------------

void Button::set_enabled(bool enabled)
{
    is_enabled_ = enabled;
    if (!is_enabled_)
    {
        is_pressed_ = false;
        is_hovered_ = false;
    }
    update_visuals();
}//!set_enabled

FLEV_NODISCARD bool Button::handle_event(const sf::Event& event)
{
    if (!is_enabled_) return false;

    clicked_ = false;
    if (auto moved = event.getIf<sf::Event::MouseMoved>())
    {
        sf::Vector2f mouse_pos(static_cast<float>(moved->position.x), static_cast<float>(moved->position.y));
        is_hovered_ = body_.getGlobalBounds().contains(mouse_pos);

        if (!is_pressed_) update_visuals();
    }
    else if (auto pressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (pressed->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mouse_pos(static_cast<float>(pressed->position.x), static_cast<float>(pressed->position.y));
            if (body_.getGlobalBounds().contains(mouse_pos))
            {
                is_pressed_ = true;
                update_visuals();
            }
        }
    }
    else if (auto released = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (released->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mouse_pos(static_cast<float>(released->position.x), static_cast<float>(released->position.y));
            bool mouse_on_button = body_.getGlobalBounds().contains(mouse_pos);
            if (is_pressed_ && mouse_on_button)
            {
                clicked_ = true;
            }
            is_pressed_ = false;
            is_hovered_ = mouse_on_button;
            update_visuals();
        }
    }

    return clicked_;
}//!handle_event
//---------------------------------------------------------------------------------------

void Button::update_visuals()
{
    if (is_pressed_)  // Active
    {
        body_.setFillColor(sf::Color(100, 100, 255));
    }
    else if (is_hovered_)  // Hover
    {
        body_.setFillColor(sf::Color(200, 200, 255));
    }
	else if (!is_enabled_)  // Disabled
    {
        body_.setFillColor(sf::Color(80, 80, 80));
    }
    else  // Idle
    {
        body_.setFillColor(sf::Color(100, 100, 100));
    }
}//!update_visuals
//---------------------------------------------------------------------------------------

void Button::draw(sf::RenderTarget& render_target) const
{
    render_target.draw(body_);
    render_target.draw(label_);
}//!draw
//---------------------------------------------------------------------------------------