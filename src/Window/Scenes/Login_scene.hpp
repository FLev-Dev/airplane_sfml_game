#pragma once
#include "Scene.hpp"
#include <UI/Panel.hpp>
#include <UI/Label.hpp>
#include <UI/Button.hpp>

class Login_scene final : public Scene
{
public:

    /** @brief Constructs the login scene with UI elements for player name input. */
    Login_scene(Main_window& window);

    /** @brief Handles text input, backspace, Enter, and button click events. */
    void handle_event(const sf::Event& event) override;

    /** @brief Updates the blinking cursor and input field display. */
    void update(const float dt) override;

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Login. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

    /**
     * @brief Converts internal sf::String (UTF-32) to std::string (UTF-8).
     * 
     * @returns The entered player name.
     */
    FLEV_NODISCARD std::string get_player_name() const;

private:

    sf::Font font_;                          ///< Font used in the scene.

	sf::RectangleShape overlay_;             ///< Semi-transparent dimming layer.
    std::unique_ptr<Panel> panel_;           ///< Central panel for UI elements.
    Label title_label_;                      ///< Static header text.
    Label input_label_;                      ///< Dynamic field showing current input and cursor.
    std::unique_ptr<Button> confirm_button_; ///< Button to confirm and proceed to main menu.

    sf::Texture background_tex_;             ///< Background texture (shared with main menu).
    std::unique_ptr<sf::Sprite> background_; ///< Background sprite (scaled to window).

    sf::String player_name_;                 ///< Player name in UTF-32 for safe Unicode input.
};