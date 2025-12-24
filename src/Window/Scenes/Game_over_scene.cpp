#include "Game_over_scene.hpp"
#include "../Main_window.hpp"

Game_over_scene::Game_over_scene(Main_window& window) : Scene(window)
{
    auto window_size = window.get_window_size();

    // Shading
    overlay_.setSize(sf::Vector2f(
        static_cast<float>(window_size.x),
        static_cast<float>(window_size.y)
    ));
    overlay_.setFillColor(sf::Color(40, 40, 60, 200));

    // Panel
    panel_ = std::make_unique<Panel>();
    panel_->set_size({ 500.f, 300.f });
    panel_->set_position({
        (window_size.x - 500.f) / 2.f,
        (window_size.y - 300.f) / 2.f
    });

	// Font
    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load font for game over scene.");
    }

	// Title label
    title_label_ = std::make_unique<Label>("Вы проиграли!", font_, 38);
    title_label_->set_color(sf::Color::White);

	// Buttons
	const auto button_size = sf::Vector2f{ 200.f, 50.f };
    buttons_.emplace("restart", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, button_size),
        "Начать заново",
        font_,
        30u
    ));
    buttons_.emplace("menu", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, button_size),
        "В меню",
        font_,
        30u
    ));
    buttons_.emplace("exit", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, button_size),
        "Выйти",
        font_,
        30u
    ));

    // Positioning objects on the panel
    const auto panel_pos = panel_->get_bounds().position;
    const auto panel_size = panel_->get_bounds().size;
    const auto panel_center = panel_->get_bounds().getCenter();

    title_label_->set_position({
        panel_pos.x + (panel_size.x - title_label_->get_bounds().size.x) / 2.f,
        panel_pos.y + 10.f
    });

    buttons_["restart"]->set_position({ panel_center.x - 100.f, panel_pos.y + 80.f });
    buttons_["menu"]->set_position({ panel_center.x - 100.f, panel_pos.y + 150.f });
    buttons_["exit"]->set_position({ panel_center.x - 100.f, panel_pos.y + 220.f });
}//!Game_over_scene
//---------------------------------------------------------------------------------------

void Game_over_scene::handle_event(const sf::Event& event)
{
    for (auto& [name, btn] : buttons_)
    {
        if (btn->handle_event(event))
        {
            if (name == "restart")
            {
                main_window_.switch_to(Game_state::Game);
            }
            else if (name == "menu")
            {
                main_window_.switch_to(Game_state::Main_Menu);
            }
            else if (name == "exit")
            {
                main_window_.close();
            }
            break;
        }
    }

	// Escape = to main menu
    if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            main_window_.switch_to(Game_state::Main_Menu);
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Game_over_scene::draw(sf::RenderTarget& render_target)
{
	// Draw last game snapshot (if exists)
    if (const auto* snapshot = main_window_.get_game_snapshot())
    {
        sf::Sprite sprite(*snapshot);
        render_target.draw(sprite);
    }

    // Shading
    render_target.draw(overlay_);

	// Panel and buttons
    panel_->draw(render_target);
    title_label_->draw(render_target);
    for (const auto& [_, btn] : buttons_)
    {
        btn->draw(render_target);
    }
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Game_over_scene::get_scene_type() const
{ 
    return Game_state::Game_over; 
}//!get_scene_type
//---------------------------------------------------------------------------------------