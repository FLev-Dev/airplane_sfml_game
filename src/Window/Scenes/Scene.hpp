#pragma once
#include <utils/defines.hpp>
#include <SFML/Graphics.hpp>
#include "Game_state.hpp"

class Main_window; // Forward declaration

/** @brief Base interface for all scenes (menu, game, pause, etc.). */
class Scene
{
public:

	/** @brief Constructs a scene with a reference to the main window. */
    Scene(Main_window& window) : main_window_(window) {}
    virtual ~Scene() = default;

	/** @brief Handles an incoming event. */
    virtual void handle_event(const sf::Event& event) = 0;

	/** @brief Updates scene logic. */
    virtual void update(float dt) = 0;

	/** @brief Draws the scene onto the given render target. */
    virtual void draw(sf::RenderTarget& render_target) = 0;

	/** @brief Returns the current type of the scene. */
    virtual FLEV_NODISCARD Game_state get_scene_type() const = 0;

protected:
	Main_window& main_window_; ///< Reference to the main window.
};