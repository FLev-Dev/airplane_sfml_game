#include "Login_scene.hpp"
#include "../main_window.hpp"
#include <algorithm>

Login_scene::Login_scene(Main_window& window): Scene(window)
{
    const auto window_size = window.get_window_size();
    
	// Background
    if (!background_tex_.loadFromFile("assets/main_menu_bg.png"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load login scene background texture.");
    }
    background_ = std::make_unique<sf::Sprite>(background_tex_);
    background_->setScale({
        static_cast<float>(window_size.x) / background_tex_.getSize().x,
        static_cast<float>(window_size.y) / background_tex_.getSize().y
    });

    // Overlay
    overlay_.setSize(sf::Vector2f(static_cast<float>(window_size.x), static_cast<float>(window_size.y)));
    overlay_.setFillColor(sf::Color(40, 40, 60, 200));

	// Font
    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load font for login scene.");
    }

    // Panel
    panel_ = std::make_unique<Panel>(sf::Vector2f{ 500.f, 300.f });
    panel_->set_position({
        (window_size.x - 500.f) / 2.f,
        (window_size.y - 300.f) / 2.f
    });
    const auto panel_bounds = panel_->get_bounds();

    // Header
    title_label_ = Label("Введите ваше имя:", font_, 40);
    title_label_.set_position({
        panel_bounds.position.x + 50.f,
        panel_bounds.position.y + 30.f
    });
    title_label_.set_color(sf::Color::White);

    // Input field
    input_label_ = Label("_", font_, 36);
    input_label_.set_position({
        panel_bounds.position.x + 50.f,
        panel_bounds.position.y + 100.f
    });
    input_label_.set_color(sf::Color::Cyan);

    // Accept button
    confirm_button_ = std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, { 200.f, 50.f }),
        "Готово",
        font_,
        30
    );
    confirm_button_->set_position({
        panel_bounds.position.x + (panel_bounds.size.x - 200.f) / 2.f,
        panel_bounds.position.y + 200.f
    });
}//!Login_scene
//---------------------------------------------------------------------------------------

void Login_scene::handle_event(const sf::Event& event)
{
    if (auto text = event.getIf<sf::Event::TextEntered>())
    {
        if (player_name_.getSize() < 6) // Max length
        {
            if (text->unicode >= 0x20 && text->unicode <= 0x10FFFF)
            {
                player_name_ += text->unicode;
            }
            
        }
    }
    else if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Backspace && !player_name_.isEmpty())
        {
            player_name_ = player_name_.substring(0, player_name_.getSize() - 1);
        }
        else if (key->code == sf::Keyboard::Key::Enter)
        {
            if (!player_name_.isEmpty())
            {
                main_window_.switch_to(Game_state::Main_Menu);
                return;
            }
        }
        else if (key->code == sf::Keyboard::Key::Escape)
        {
            main_window_.close();
        }
    }

    if (confirm_button_->handle_event(event))
    {
        if (!player_name_.isEmpty())
        {
            main_window_.switch_to(Game_state::Main_Menu);
            return;
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Login_scene::update(const float dt)
{
	// Blinking cursor logic
    static float blink_timer = 0.f;
    blink_timer += dt;
    char cursor = (blink_timer > 0.5f) ? '_' : ' ';
    if (blink_timer > 1.f) blink_timer = 0.f;

    auto utf8 = player_name_.toUtf8();
    std::string display_text = player_name_.isEmpty() ? "Имя..." : std::string(utf8.begin(), utf8.end());
    input_label_.set_text(display_text + cursor);
}//!update
//---------------------------------------------------------------------------------------

void Login_scene::draw(sf::RenderTarget& render_target)
{
	render_target.draw(*background_);
    render_target.draw(overlay_);

    panel_->draw(render_target);
    title_label_.draw(render_target);
    input_label_.draw(render_target);
    confirm_button_->draw(render_target);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Login_scene::get_scene_type() const
{ 
    return Game_state::Login; 
}//!get_scene_type
//---------------------------------------------------------------------------------------

FLEV_NODISCARD std::string Login_scene::get_player_name() const
{
    auto utf8 = player_name_.toUtf8();
	return std::string(utf8.begin(), utf8.end());
}//!get_player_name
//---------------------------------------------------------------------------------------