#include "Label.hpp"

Label::Label(const std::string& text, const sf::Font& font, uint32_t char_size)
{
	set_font(font);
    set_text(text);
    set_char_size(char_size);
}//!Label
//---------------------------------------------------------------------------------------

void Label::set_text(const std::string& text)
{
    if(text_) text_->setString(sf::String::fromUtf8(text.begin(), text.end()));
}//!set_text
//---------------------------------------------------------------------------------------

void Label::set_position(const sf::Vector2f& position)
{
    if (text_) text_->setPosition(position);
}//!set_position
//---------------------------------------------------------------------------------------

void Label::set_color(const sf::Color color)
{
    if (text_) text_->setFillColor(color);
}//!set_color
//---------------------------------------------------------------------------------------

void Label::set_char_size(const uint32_t size)
{
    if (text_) text_->setCharacterSize(size);
}//!set_char_size
//---------------------------------------------------------------------------------------

void Label::set_font(const sf::Font& font)
{
    if (text_) text_->setFont(font);
    else text_ = std::make_unique<sf::Text>(font);
}//!set_font
//---------------------------------------------------------------------------------------

void Label::draw(sf::RenderTarget& render_target) const
{
    render_target.draw(*text_);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::FloatRect Label::get_bounds() const
{
    return text_ ? text_->getGlobalBounds() : sf::FloatRect{};
}//!get_bounds
//---------------------------------------------------------------------------------------