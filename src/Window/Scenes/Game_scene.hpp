#pragma once
#include <UI/Label.hpp>
#include <UI/Panel.hpp>
#include <UI/Button.hpp>
#include "Scene.hpp"
#include <Entities/Enemy.hpp>
#include <Entities/Player.hpp>
#include <Entities/Bullet.hpp>
#include <vector>
#include <memory>

class Game_scene final : public Scene
{
public:

    /** @brief Constructs game scene for the given level. */
    Game_scene(Main_window& window, const int32_t level_id);

    /** @brief Handles pause menu events and Escape key. */
    void handle_event(const sf::Event& event) override;

    /** @brief Updates all game logic, collisions, and win/lose conditions. */
    void update(const float dt) override;

    /** @brief Draws UI, game objects, and pause overlay if needed. */
    void draw(sf::RenderTarget& render_target) override;

    /** @brief Returns Game_state::Game. */
    FLEV_NODISCARD Game_state get_scene_type() const override;

private/*methods*/:

    /** @brief Initializes parallax background based on level. */
    void initialize_sky(const sf::Vector2u& window_size);

    /** @brief Initializes UI elements: hearts, controls, labels. */
    void initialize_ui(const sf::Vector2u& window_size);

    /** @brief Sets up pause menu panel and buttons. */
    void initialize_pause_menu(const sf::Vector2u& window_size);

    /**
     * @brief Draws only gameplay entities (sky, player, enemies, bullets).
     *
     * Used for both main rendering and game over screenshot.
     */
    void draw_game_objects(sf::RenderTarget& render_target);

    /** @brief Spawns enemies according to current level rules. */
    void spawn_enemy();

    /** @brief Updates scrolling sky (level 0 only). */
    void update_sky(const float dt);

private/*vars*/:

    // -----------------------------------------------------------------------
    // Level state
    // -----------------------------------------------------------------------
    const int32_t current_level_id_; ///< Current level ID.
    float level_duration_ = 120.f;   ///< Time limit for timed levels (seconds).
    sf::Clock level_timer_;          ///< Elapsed time since level start.

    // -----------------------------------------------------------------------
    // Game entities
    // -----------------------------------------------------------------------
    Player player_;                                                      ///< Player ship.
    std::map<std::string, std::vector<std::unique_ptr<Enemy>>> enemies_; ///< Enemies by type.
    std::vector<std::unique_ptr<Bullet>> bullets_;                       ///< Player-fired bullets.
    std::vector<std::unique_ptr<Bullet>> enemy_bullets_;                 ///< Enemy-fired bullets.

    // -----------------------------------------------------------------------
    // Game state
    // -----------------------------------------------------------------------
    int32_t score_ = 0;                   ///< Current score (accumulated during level).
    uint16_t total_scout_enemies_ = 16u;  ///< Remaining scout enemies (level 1).
    uint16_t total_warrior_enemies_ = 8u; ///< Remaining warrior enemies (level 1).

    // -----------------------------------------------------------------------
    // UI resources
    // -----------------------------------------------------------------------
    sf::Font ui_font_;                                      ///< Font for all on-screen text.

    std::map<std::string, sf::Texture> ui_textures_;        ///< Loaded UI textures (hearts, controls, backgrounds).

    std::vector<std::unique_ptr<sf::Sprite>> sky_sprites_;  ///< Background parallax layers.
    std::vector<std::unique_ptr<sf::Sprite>> health_icons_; ///< Player health indicators (full/empty).
    std::unique_ptr<sf::Sprite> controls_;                  ///< Controls hint icon.

    // -----------------------------------------------------------------------
    // UI elements
    // -----------------------------------------------------------------------
    Label win_cond_label_; ///< Dynamic win condition text (timer/enemy count/etc...).

    // -----------------------------------------------------------------------
    // Pause menu
    // -----------------------------------------------------------------------
    bool paused_ = false;                                          ///< Whether game logic is paused.
    sf::RectangleShape pause_overlay_;                             ///< Semi-transparent dimming layer.
    std::unique_ptr<Panel> pause_panel_;                           ///< Pause menu background panel.
    std::map<std::string, std::unique_ptr<Button>> pause_buttons_; ///< Pause menu buttons.

    // -----------------------------------------------------------------------
    // Timing
    // -----------------------------------------------------------------------
    float spawn_time_ = 5.f; ///< Base enemy spawn interval (seconds).
    sf::Clock spawn_clock_;  ///< Timer for next enemy spawn.
    sf::Clock fps_clock_;    ///< Unused (for debugging).
};