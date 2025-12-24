#pragma once
#include "Level/Progress_manager.hpp"
#include "Scenes/Scene.hpp"
#include <utils/database_api.hpp>
#include <utils/defines.hpp>
#include <memory>
#include <map>

/** @brief Main application window and scene manager. */
class Main_window
{
public:

    /** @brief Constructs the window with optional size [Default: 1920x1080]. */
    Main_window(const sf::Vector2u& window_size = { 1920u, 1080u });

    /** @brief Saves progress on destruction. */
    ~Main_window() noexcept;

    /** @brief Main game loop: event handling, update, draw. */
    void run();

    /** @brief Switches to a named game state (creates corresponding scene). */
    void switch_to(const Game_state state);

    /** @brief Switches to victory screen and saves score to database. */
    void switch_to_victory(const int32_t score);

    /** @brief Switches to game scene for the specified level. */
    void switch_to_game(const int32_t level_id);

    /** @brief Returns currently active player name (set after login). */
    FLEV_NODISCARD std::string get_player_name();

    /** @brief Checks if the given level is unlocked via progress manager. */
    FLEV_NODISCARD bool is_level_unlocked(const int32_t level_id) const;

    /** @brief Returns the highest level ID available in the game. */
    FLEV_NODISCARD int32_t get_max_level_id() const;

    /** @brief Unlocks the next level (current_level_id_ + 1). */
    FLEV_NODISCARD bool unlock_next_level();

    /** @brief Requests application shutdown. */
    void close();

    /** @brief Returns reference to SFML render window. */
    FLEV_NODISCARD sf::RenderWindow& get_window();

    /** @brief Returns current window size. */
    FLEV_NODISCARD sf::Vector2u get_window_size() const;

    /** @brief Returns render target for game over screenshot. */
    FLEV_NODISCARD sf::RenderTexture& get_snapshot_target();

    /** @brief Returns last game frame texture (for GameOverScene). */
    FLEV_NODISCARD const sf::Texture* get_game_snapshot() const;

private/*methods*/:

    /** @brief Loads leaderboard data from DB and creates Leaderboard_scene. */
    FLEV_NODISCARD void create_leaderboard_scene();

private/*vars*/:

    // -----------------------------------------------------------------------
    // Window and lifecycle
    // -----------------------------------------------------------------------
    sf::RenderWindow window_;     ///< SFML application window.
    bool should_close_ = false;   ///< Shutdown flag.

    // -----------------------------------------------------------------------
    // Game state
    // -----------------------------------------------------------------------
    std::unique_ptr<Scene> current_scene_; ///< Currently active scene.
    Game_state current_state_;             ///< Current game state enum.
    std::string player_name_;              ///< Player name (set after login).
    int32_t current_level_id_ = 0;         ///< Level ID for next Game_scene.

    // -----------------------------------------------------------------------
    // Persistence
    // -----------------------------------------------------------------------
    std::unique_ptr<Database> db;           ///< SQLite database connection.
    Progress_manager progress_manager_;     ///< Tracks unlocked levels (file-based).

    // -----------------------------------------------------------------------
    // Rendering
    // -----------------------------------------------------------------------
    sf::RenderTexture game_snapshot_;       ///< Last game frame.
};