#pragma once
#include "Scene.hpp"
#include <UI/Label.hpp>
#include <UI/Decorated_panel.hpp>
#include <vector>

struct Leaderboard_entry; ///< Forward declaration.

class Leaderboard_scene final : public Scene
{
    /** @brief Represents a logical row in the scrollable leaderboard view. */
    struct Virtual_row
    {
        enum class Type { Player_header, Level_detail } type; ///< Row type.
        size_t player_index = 0;                              ///< Index in entries_.
        int32_t level_id    = -1;                             ///< Valid only for Level_detail.
    };

public:

    /** @brief Constructs the leaderboard scene with provided score data. */
    Leaderboard_scene(Main_window& window, const std::vector<Leaderboard_entry>& entries);

    /** @brief Handles mouse wheel scrolling and Escape key. */
    void handle_event(const sf::Event& event) override;

    /** @brief No dynamic logic; scroll offset handled in handle_event. */
    void update(const float) override {}

    /** @brief Draws the scene onto the given render target. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Leaderboard. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private/*methods*/:

    /** @brief Renders visible leaderboard entries with proper scrolling and hierarchy. */
    void render_entries(sf::RenderTarget& render_target);

    /** @brief Builds flat list of virtual rows (player headers + level details). */
    FLEV_NODISCARD std::vector<Virtual_row> build_virtual_rows() const;


private/*vars*/:

    sf::Font font_;                                   ///< Font for all text elements.

    sf::Texture background_tex_;                      ///< Background texture (main menu style).
    std::unique_ptr<sf::Sprite> background_;          ///< Scaled background sprite.
    sf::RectangleShape overlay_;                      ///< Semi-transparent dimming layer.
    std::unique_ptr<Decorated_panel> panel_;          ///< Panel with title and content area.
    std::vector<Leaderboard_entry> entries_;          ///< Source leaderboard data.

    float scroll_offset_ = 0.f;                       ///< Vertical scroll position (in pixels).
    float entry_height_ = 40.f;                       ///< Height of a single logical row.
    uint32_t visible_entries_ = 8;                    ///< Max number of rows fitting in panel.

    mutable std::vector<Virtual_row> virtual_rows_cache_; ///< Cached flat view of entries.
    mutable bool virtual_rows_dirty_ = true;              ///< Flag to rebuild cache.
};