#include "Level_selection.hpp"
#include "../Main_window.hpp"
#include <utils/logger.hpp>

Level_selection_scene::Level_selection_scene(Main_window& window)
    : Scene(window)
{

    const auto window_size = window.get_window_size();

	// Background
    if (!background_tex_.loadFromFile("assets/main_menu_bg.png"))
    {
        LOG_ERROR(get_global_logger(), "Failed to load background texture for level_selection.");
    }
    background_ = std::make_unique<sf::Sprite>(background_tex_);
    background_->setScale({
        static_cast<float>(window_size.x) / background_tex_.getSize().x,
        static_cast<float>(window_size.y) / background_tex_.getSize().y
    });

	// Shading overlay
    overlay_.setSize(sf::Vector2f(
        static_cast<float>(window_size.x),
        static_cast<float>(window_size.y)
    ));
    overlay_.setFillColor(sf::Color(40, 40, 60, 200));

    // Font
    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load font for level selection scene.");   
    }

	// Level buttons
    const auto num_levels = window.get_max_level_id() + 1;
    level_buttons_.reserve(num_levels);

    TODO("create panel with calucalated size")
    panel_ = std::make_unique<Panel>(sf::Vector2f{ 500.f, 400.f });
    panel_->set_position({
        (window_size.x - 500.f) / 2.f,
        (window_size.y - 400.f) / 2.f
    });

    const auto panel_bounds = panel_->get_bounds();
    const float button_width = 400.f;
    const float button_height = 60.f;
    const float start_y = panel_bounds.position.y + 50.f;

    for (int32_t i = 0; i < num_levels; i++)
    {
        std::string label = "Уровень " + std::to_string(i + 1);
        auto btn = std::make_unique<Button>(
            sf::FloatRect({0, 0}, {button_width, button_height}),
            label,
            font_,
            32u
        );

        btn->set_position({
            panel_bounds.position.x + (panel_bounds.size.x - button_width) / 2.f,
            start_y + i * (button_height + 15.f)
        });

        level_buttons_.push_back(std::move(btn));
    }

    back_button_ = std::make_unique<Button>(
        sf::FloatRect({0, 0}, {150.f, 40.f}),
        "Назад",
        font_,
        24u
    );
    back_button_->set_position({
        panel_bounds.position.x + (panel_bounds.size.x - 150.f) / 2.f,
        panel_bounds.position.y + panel_bounds.size.y - 60.f
    });

	// Actualize button states based on unlocked levels
    update_button_states();
}//!Level_selection_scene
//---------------------------------------------------------------------------------------

void Level_selection_scene::update_button_states()
{
    const auto num_levels = static_cast<int32_t>(level_buttons_.size());
    for (int32_t i = 0; i < num_levels; ++i)
    {
        const bool unlocked = main_window_.is_level_unlocked(i);
        level_buttons_[i]->set_enabled(unlocked);

        if (unlocked)
        {
            level_buttons_[i]->set_text_color(sf::Color::Cyan);
        }
        else
        {
            level_buttons_[i]->set_text_color(sf::Color(150, 150, 150));
        }
    }
}//!update_button_states
//---------------------------------------------------------------------------------------

void Level_selection_scene::handle_event(const sf::Event& event)
{
    if (back_button_->handle_event(event))
    {
        main_window_.switch_to(Game_state::Main_Menu);
        return;
    }

    for (size_t i = 0; i < level_buttons_.size(); ++i)
    {
        if (level_buttons_[i]->handle_event(event))
        {
            main_window_.switch_to_game(static_cast<int32_t>(i));
            return;
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

void Level_selection_scene::draw(sf::RenderTarget& render_target)
{
	render_target.draw(*background_);
    render_target.draw(overlay_);
	panel_->draw(render_target);

    for (const auto& btn : level_buttons_)
    {
        btn->draw(render_target);
    }
    back_button_->draw(render_target);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Level_selection_scene::get_scene_type() const
{ 
    return Game_state::Level_Selection; 
}//!get_scene_type
//---------------------------------------------------------------------------------------