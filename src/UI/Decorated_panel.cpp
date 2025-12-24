#include "Decorated_panel.hpp"
#include <vector>

Decorated_panel::Decorated_panel(
    const std::string& texture_path,
    const sf::Vector2f& size
)
    : Panel(size, sf::Color::Transparent)
{
    static std::vector<std::unique_ptr<sf::Texture>> textures;

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(texture_path))
    {
		LOG_ERROR(get_global_logger(), "Failed to load decorated panel texture: {}", texture_path);
        return;
    }

    background_sprite_ = std::make_unique<sf::Sprite>(*texture);
    textures.push_back(std::move(texture));

    set_size(size);
}//!Decorated_panel
//---------------------------------------------------------------------------------------

void Decorated_panel::set_size(const sf::Vector2f& size)
{
    if (background_sprite_)
    {
        sf::Vector2u texture_size = background_sprite_->getTexture().getSize();
        const float scale_x = size.x / texture_size.x;
        const float scale_y = size.y / texture_size.y;
        background_sprite_->setScale({ scale_x, scale_y });

		// Scale content bounds
        current_content_bounds_ = sf::FloatRect(
            {
                background_sprite_->getPosition().x + content_rect_px_.position.x * scale_x,
                background_sprite_->getPosition().y + content_rect_px_.position.y * scale_y
            },
            {
                content_rect_px_.size.x * scale_x,
                content_rect_px_.size.y * scale_y
            }
        );

		// Scale title position
        current_title_center_ = sf::Vector2f(
            background_sprite_->getPosition().x + (texture_size.x / 2.f) * scale_x,
            background_sprite_->getPosition().y + title_y_px_ * scale_y
        );
    }

    Panel::set_size(size);
}//!set_size
//---------------------------------------------------------------------------------------

void Decorated_panel::set_position(const sf::Vector2f& position)
{
    if (background_sprite_) background_sprite_->setPosition(position);

    Panel::set_position(position);

    if (background_sprite_) 
    {
        const auto texture_size = background_sprite_->getTexture().getSize();
        const auto scale_x = background_sprite_->getScale().x;
        const auto scale_y = background_sprite_->getScale().y;

        current_content_bounds_.position.x = position.x + content_rect_px_.position.x * scale_x;
        current_content_bounds_.position.y = position.y + content_rect_px_.position.y * scale_y;

        current_title_center_.x = position.x + (texture_size.x / 2.f) * scale_x;
        current_title_center_.y = position.y + title_y_px_ * scale_y;
    }

    if (title_text_)
    {
        const auto title_bounds = title_text_->getLocalBounds();
        title_text_->setPosition({
            current_title_center_.x - title_bounds.size.x / 2.f,
            current_title_center_.y - title_bounds.size.y / 2.f
        });
    }
}//!set_position
//---------------------------------------------------------------------------------------

void Decorated_panel::set_title(const std::string& title, const sf::Font& font, uint32_t char_size)
{
    title_text_ = std::make_unique<sf::Text>(font);
    title_text_->setString(sf::String::fromUtf8(title.begin(), title.end()));
    title_text_->setCharacterSize(char_size);
    title_text_->setFillColor(sf::Color::White);
    title_text_->setOutlineColor(sf::Color::Black);
    title_text_->setOutlineThickness(1.f);

    if (background_sprite_)
    {
        const sf::FloatRect title_bounds = title_text_->getLocalBounds();
        title_text_->setPosition({
            current_title_center_.x - title_bounds.size.x / 2.f,
            current_title_center_.y - title_bounds.size.y / 2.f
        });
    }
}//!set_title
//---------------------------------------------------------------------------------------

void Decorated_panel::draw(sf::RenderTarget& render_target) const
{
    if (background_sprite_) render_target.draw(*background_sprite_);
    if (title_text_)        render_target.draw(*title_text_);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::FloatRect Decorated_panel::get_content_bounds() const
{
    return current_content_bounds_;
}//!get_content_bounds
//---------------------------------------------------------------------------------------