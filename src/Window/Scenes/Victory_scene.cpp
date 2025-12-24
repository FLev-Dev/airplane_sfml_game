#include "Victory_scene.hpp"
#include "../Main_window.hpp"
#include <utils/database_api.hpp>

Victory_scene::Victory_scene(Main_window& window, const int32_t level_id, const int32_t score)
    : Scene(window), level_id_(level_id), score_(score), player_name_(main_window_.get_player_name())
{
    const auto window_size = window.get_window_size();

	// Shading overlay
    overlay_.setSize(sf::Vector2f(static_cast<float>(window_size.x), static_cast<float>(window_size.y)));
    overlay_.setFillColor(sf::Color(20, 40, 20, 220)); // Dark green

    // Panel
    panel_ = std::make_unique<Panel>();
	panel_->set_size({ 700.f, 400.f });
    panel_->set_position({
        (window_size.x - 700.f) / 2.f,
        (window_size.y - 400.f) / 2.f
    });
    const auto panel_pos = panel_->get_bounds().position;
    const auto panel_size = panel_->get_bounds().size;

    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
        LOG_ERROR(
            get_global_logger(),
			"Failed to load font for victory scene"
        );
    }

    // Title label
    title_label_ = std::make_unique<Label>("Победа!", font_, 50);
    title_label_->set_color(sf::Color::Yellow);

	// Result label
    const auto result_text = std::format("{} набрал {} очков", player_name_, score);
    result_label_ = std::make_unique<Label>(result_text, font_, 32);
    result_label_->set_color(sf::Color::White);

    // Buttons
    const sf::FloatRect button_rect = { { 0.f, 0.f }, { 180.f, 50.f } };

    buttons_.emplace("menu", std::make_unique<Button>(
        button_rect,
        "Выйти",
        font_,
        28u
    ));
    buttons_.emplace("restart", std::make_unique<Button>(
        button_rect,
        "Ещё раз",
        font_,
        28u
    ));
    buttons_.emplace("next", std::make_unique<Button>(
        button_rect,
        "Следующий",
        font_,
        28u
    ));

    // Positioning objects on the panel
    const auto panel_center = panel_->get_bounds().getCenter();
    const auto button_y = panel_pos.y + 280.f;
    
    title_label_->set_position({
        panel_->get_bounds().position.x + 50.f,
        panel_->get_bounds().position.y + 40.f
    });
    result_label_->set_position({
        panel_->get_bounds().position.x + 50.f,
        panel_->get_bounds().position.y + 120.f
    });

    buttons_["menu"]->set_position({ panel_center.x - button_rect.size.x - 100.f, button_y });
    buttons_["restart"]->set_position({ panel_center.x - button_rect.size.x / 2.f, button_y });
    buttons_["next"]->set_position({ panel_center.x + 100.f, button_y });

	// Unlock next level
    if (!main_window_.unlock_next_level())
    {
		buttons_.erase("next");
    }
}//!Victory_scene
//---------------------------------------------------------------------------------------

void Victory_scene::handle_event(const sf::Event& event)
{
    for (auto& [name, btn] : buttons_)
    {
        if (btn->handle_event(event))
        {
            if (name == "menu")
            {
                main_window_.switch_to(Game_state::Main_Menu);
            }
            else if (name == "restart")
            {
                main_window_.switch_to(Game_state::Game);
            }
            else if (name == "next")
            {
                main_window_.switch_to_game(level_id_ + 1);
            }
            break;
        }
    }

    if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            main_window_.switch_to(Game_state::Main_Menu);
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Victory_scene::draw(sf::RenderTarget& render_target)
{
    render_target.draw(overlay_);
    panel_->draw(render_target);
    title_label_->draw(render_target);
    result_label_->draw(render_target);
    for (const auto& [_, btn] : buttons_)
    {
        btn->draw(render_target);
    }
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Victory_scene::get_scene_type() const
{ 
    return Game_state::Victory; 
}//!get_scene_type
//---------------------------------------------------------------------------------------