#include "Main_menu.hpp"
#include "../Main_window.hpp"

Main_menu::Main_menu(Main_window& window, const std::string& player_name) : Scene(window)
{
    const auto window_size = window.get_window_size();

	// Background
    if (!background_tex_.loadFromFile("assets/main_menu_bg.png"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load main menu background texture.");
    }
    background_ = std::make_unique<sf::Sprite>(background_tex_);
    background_->setScale({
        static_cast<float>(window_size.x) / background_tex_.getSize().x,
        static_cast<float>(window_size.y) / background_tex_.getSize().y
    });

	// Font
    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load font for main menu.");
	}

	// Buttons
	const sf::Vector2f object_size = { 250.f, 50.f };
    player_name_ = std::make_unique<Button>(
        sf::FloatRect({ window_size.x / 10.f, window_size.y / 2.f - 300.f }, object_size),
        "Игрок: " + player_name,
        font_
    );
	player_name_->set_text_color(sf::Color::Green);

    buttons_.emplace("continue", std::make_unique<Button>(
        sf::FloatRect({ window_size.x / 10.f, window_size.y / 2.f - 200.f }, object_size),
        "Продолжить",
        font_
    ));

    buttons_.emplace("start", std::make_unique<Button>(
        sf::FloatRect({ window_size.x / 10.f, window_size.y / 2.f - 100.f }, object_size),
        "Начать игру", 
        font_
    ));
    
    buttons_.emplace("leaderboard", std::make_unique<Button>(
        sf::FloatRect({ window_size.x / 10.f, window_size.y / 2.f }, object_size),
        "Таблица лидеров",
        font_
    ));

    buttons_.emplace("exit", std::make_unique<Button>(
        sf::FloatRect({ window_size.x / 10.f, window_size.y / 2.f + 100.f }, object_size),
        "Выход",
        font_
    ));
}//!Main_menu
//---------------------------------------------------------------------------------------

void Main_menu::handle_event(const sf::Event& event)
{
    for (auto& [name, btn] : buttons_)
    {
        if (btn->handle_event(event))
        {
            if (name == "continue")
            {
                main_window_.switch_to(Game_state::Game);
            }
            else if (name == "start")
            {
                main_window_.switch_to(Game_state::Level_Selection);
            }
            else if (name == "exit")
            {
                main_window_.close();
            }
            if (name == "leaderboard")
            {
                main_window_.switch_to(Game_state::Leaderboard);
            }
            break;
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Main_menu::draw(sf::RenderTarget& render_target)
{
    render_target.draw(*background_);
    player_name_->draw(render_target);
	for (const auto& [_, button] : buttons_) button->draw(render_target);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Main_menu::get_scene_type() const
{ 
    return Game_state::Main_Menu; 
}//!get_scene_type
//---------------------------------------------------------------------------------------