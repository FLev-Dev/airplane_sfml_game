#pragma once
#include "scene.hpp"
#include <UI/Panel.hpp>
#include <UI/Label.hpp>
#include <UI/Button.hpp>
#include <map>
#include <memory>

class Game_over_scene final : public Scene
{
public:

    /** @brief Constructs the game over screen with restart/menu/exit options. */
    Game_over_scene(Main_window& window);

    /** @brief Handles button clicks and Escape key to navigate away. */
    void handle_event(const sf::Event& event) override;

    /** @brief No dynamic logic required for static result screen. */
    void update(const float) override {}

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Game_over. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private:

    sf::Font font_;                                          ///< Font for all text elements.

    sf::RectangleShape overlay_;                             ///< Semi-transparent dimming layer over game snapshot
    std::unique_ptr<Panel> panel_;                           ///< Central UI panel.
    std::unique_ptr<Label> title_label_;                     ///< Static header.
    std::map<std::string, std::unique_ptr<Button>> buttons_; ///< Navigation buttons by action name..
};