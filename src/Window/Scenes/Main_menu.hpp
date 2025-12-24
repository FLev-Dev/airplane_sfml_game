#pragma once
#include "scene.hpp"
#include <UI/Label.hpp>
#include <UI/Button.hpp>
#include <map>

class Main_menu final : public Scene
{
public:

    /** @brief Constructs the main menu with player name and navigation buttons. */
    Main_menu(Main_window& window, const std::string& player_name);

    /** @brief Handles button click events for menu navigation. */
    void handle_event(const sf::Event& event) override;

    /** @brief No logic update required for static menu. */
    void update(const float) override {}

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Main_Menu. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private:

    sf::Font font_; ///< Font used for all text elements.

    sf::Texture background_tex_;             ///< Background texture.
    std::unique_ptr<sf::Sprite> background_; ///< Scaled background sprite.

    std::unique_ptr<Button> player_name_;    ///< Non-interactive display of player name.
    std::map<std::string, std::unique_ptr<Button>> buttons_; ///< Interactive menu buttons by action name.
};