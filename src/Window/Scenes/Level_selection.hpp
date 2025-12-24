#pragma once
#include "Scene.hpp"
#include <UI/Panel.hpp>
#include <UI/Button.hpp>
#include <vector>

class Level_selection_scene final : public Scene
{
public:

    /** @brief Constructs the level selection scene with unlockable level buttons. */
    Level_selection_scene(Main_window& window);

    /** @brief Handles "Back" button and level selection events. */
    void handle_event(const sf::Event& event) override;

    /** @brief No dynamic logic required for static level list. */
    void update(const float) override {}

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Level_Selection. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private/*methods*/:

    /** @brief Updates button colors and interactivity based on unlock status. */
    void update_button_states();


private/*vars*/:

    sf::Font font_; ///< Font for all text elements.

    sf::Texture background_tex_;             ///< Background texture (main menu style).
    std::unique_ptr<sf::Sprite> background_; ///< Scaled background sprite.
    sf::RectangleShape overlay_;             ///< Semi-transparent dimming layer.

    std::unique_ptr<Panel> panel_;                       ///< Central panel containing level buttons.
    std::vector<std::unique_ptr<Button>> level_buttons_; ///< One button per level (active/inactive).
    std::unique_ptr<Button> back_button_;                ///< Navigation back to main menu.
};