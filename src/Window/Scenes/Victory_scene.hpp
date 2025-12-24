#pragma once
#include "scene.hpp"
#include <UI/Label.hpp>
#include <UI/Panel.hpp>
#include <UI/Button.hpp>
#include <map>

class Victory_scene : public Scene
{
public:

    /**
     * @brief Constructs the victory screen showing player score and level completion.
     *
     * @note Automatically unlocks the next level and conditionally shows "Next" button.
     */
    Victory_scene(Main_window& window, const int32_t level_id, const int32_t score);

    /** @brief Handles navigation button clicks and Escape key. */
    void handle_event(const sf::Event& event) override;

    /** @brief No dynamic logic required for static victory screen. */
    void update(const float) override {}

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Victory. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private:

    int32_t level_id_;                  ///< Completed level ID (used for "Next" button).
    int32_t score_;                     ///< Total score achieved on this level.
    std::string player_name_;           ///< Player name displayed in result message.

    sf::Font font_;                         ///< Font for all text elements.

    sf::RectangleShape overlay_;            ///< Dimming layer (victory theme).
    std::unique_ptr<Panel> panel_;          ///< Central UI panel.
    std::unique_ptr<Label> title_label_;    ///< Static header.
    std::unique_ptr<Label> result_label_;   ///< Static message.
    std::map<std::string, std::unique_ptr<Button>> buttons_; ///< Action buttons by name.
};