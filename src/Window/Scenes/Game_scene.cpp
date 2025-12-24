#include "Game_scene.hpp"
#include "../Main_window.hpp"

#include "Entities/Small_stone.hpp"
#include "Entities/Big_stone.hpp"
#include "Entities/Scout.hpp"
#include "Entities/Warrior.hpp"

#include <utils/debug_bounds.hpp>
#include <random>

Game_scene::Game_scene(Main_window& window, const int32_t level_id): 
    Scene(window), current_level_id_(level_id)
{
    switch (current_level_id_)
    {
    case 0:
        level_duration_ = 120.f;
        spawn_time_ = 1.5f; 
		break;

    default:
        level_duration_ = 0.f;
        spawn_time_ = 3.f;
		break;
    }

    // Player
    auto window_size = window.get_window_size();
    player_.set_position(sf::Vector2f(window_size.x / 6.f, window_size.y / 2.f));

	// UI
    initialize_sky(window_size);
    initialize_ui(window_size);
    initialize_pause_menu(window_size);

    // Level begins
    level_timer_.restart();
}//!Game_scene
//---------------------------------------------------------------------------------------

void Game_scene::handle_event(const sf::Event& event)
{
    for (auto& [name, btn] : pause_buttons_)
    {
        if (btn->handle_event(event))
        {
            if (name == "resume")
            {
                paused_ = !paused_;
            }
            else if (name == "menu")
            {
                main_window_.switch_to(Game_state::Main_Menu);
            }
            else if (name == "exit")
            {
                main_window_.close();
            }
            break;
        }
    }

    if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            paused_ = !paused_;
            return;
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Game_scene::update(const float dt)
{
    if (paused_) return;
    const auto window_size = main_window_.get_window_size();

    // Game over
    if (!player_.is_alive())
    {
		// Render screenshot for game over scene
        auto& target = main_window_.get_snapshot_target();
        if (!target.resize(window_size))
        {
			LOG_ERROR(get_global_logger(), "Failed to resize render texture for game over screenshot.");
        }
        target.clear();
        for (const auto& sky : sky_sprites_) { target.draw(*sky); }
        win_cond_label_.draw(target);
        draw_game_objects(target);
        target.display();

		// Switch to game over scene
        main_window_.switch_to(Game_state::Game_over);
        return;
    }

    // Enemies spawn
    if (spawn_clock_.getElapsedTime().asSeconds() > spawn_time_)
    {
        spawn_enemy();
        spawn_clock_.restart();
    }

	// Bullets spawn
    if (player_.is_need_to_shoot())
    {
        const auto pb = player_.get_bounds();
        bullets_.push_back(std::make_unique<Bullet>(
            sf::Vector2f(pb.position.x + pb.size.x, pb.position.y + pb.size.y / 2.f),
            sf::Vector2f(800.f, 0.f)
        ));
    }

    // Background update
    update_sky(dt);
    
    // Timer update
    if (level_duration_ > 0)
    {
        const auto remaining = static_cast<int32_t>(level_duration_ - level_timer_.getElapsedTime().asSeconds());
        if (remaining <= 0)
        {
            // Victory
            score_ += player_.get_hp() * 10; // Bonus for remaining health
            main_window_.switch_to_victory(score_);
            return;
        }
        win_cond_label_.set_text(std::format("До станции осталось: {} миль.", std::max(0, remaining / 2)));
    }
    // Level 1 win condition
    if (current_level_id_ == 1)
    {
        const auto total_enemies = total_scout_enemies_ + total_warrior_enemies_;
        win_cond_label_.set_text(std::format("Противников осталось: {}", total_enemies));
        if (total_enemies <= 0)
        {
            // Victory
			score_ += (200 - level_timer_.getElapsedTime().asSeconds()) * 3; // Bonus for speed
            score_ += player_.get_hp() * 10; // Bonus for remaining health
            main_window_.switch_to_victory(score_);
            return;
        }
    }

	// Player update
    player_.update(dt, window_size);


	// Bullets update
    for (auto it = bullets_.begin(); it != bullets_.end(); )
    {
        (*it)->update(dt);
        if ((*it)->is_out_of_bounds(window_size))
        {
            it = bullets_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Enemy bullets update
    for (auto it = enemy_bullets_.begin(); it != enemy_bullets_.end(); )
    {
        (*it)->update(dt);
        if ((*it)->is_out_of_bounds(window_size))
        {
            it = enemy_bullets_.erase(it);
        }
        else
        {
            ++it;
        }
    }

	// Enemies update
    for (auto& [type, enemies] : enemies_)
    {
        for (auto it = enemies.begin(); it != enemies.end(); )
        {
            (*it)->update(dt, window_size);

            // Enemy shooting
            if (type == "warrior")
            {
                Warrior* warrior_enemy = dynamic_cast<Warrior*>(it->get());
                if (warrior_enemy->is_need_to_shoot())
                {
                    const auto enemy_bounds = warrior_enemy->get_bounds();
                    enemy_bullets_.push_back(std::make_unique<Bullet>(
                        sf::Vector2f(enemy_bounds.position.x, enemy_bounds.position.y + enemy_bounds.size.y / 2.f),
                        sf::Vector2f(-600.f, 0.f),
                        "assets/enemy_bullet1.png"
                    ));
                }
            }

            if ((*it)->is_out_of_bounds(window_size))
            {
                it = enemies.erase(it);
            }
            else
            {
				// Collision with player
                if ((*it)->get_bounds().findIntersection(player_.get_bounds()))
                {
                    const auto old_hp = player_.get_hp();
                    const auto is_player_dead = player_.take_damage(1);
                    const auto new_hp = player_.get_hp();
                    if (new_hp < old_hp && new_hp >= 0)
                    {
                        health_icons_[new_hp]->setTexture(ui_textures_["heart_empty"]);
                    }

                    if (type == "scout") total_scout_enemies_--;
					else if (type == "warrior") total_warrior_enemies_--;

                    if (is_player_dead)
                    {
                        // Do not erase enemy if player is dead (for game over screen)
                        it++;
                    }
                    else
                    {
                        it = enemies.erase(it);
                    }
                }
                else
                {
                    it++;
                }
            }
        }
    }

	// Bullet-bullet collisions (player vs enemy)
    for (auto bullet_it = bullets_.begin(); bullet_it != bullets_.end(); )
    {
        bool hit = false;
        for (auto enemy_bullet_it = enemy_bullets_.begin(); enemy_bullet_it != enemy_bullets_.end(); )
        {
            if ((*bullet_it)->get_bounds().findIntersection((*enemy_bullet_it)->get_bounds()))
            {
                enemy_bullet_it = enemy_bullets_.erase(enemy_bullet_it);
                hit = true;
                break;
            }
            else
            {
                ++enemy_bullet_it;
            }
        }
        if (hit)
        {
            bullet_it = bullets_.erase(bullet_it);
        }
        else
        {
            ++bullet_it;
        }
	}

	// Bullet-enemy collisions
    for (auto bullet_it = bullets_.begin(); bullet_it != bullets_.end(); )
    {
        bool hit = false;
        for (auto& [type, enemies] : enemies_)
        {
            for (auto enemy_it = enemies.begin(); enemy_it != enemies.end(); )
            {
                if ((*bullet_it)->get_bounds().findIntersection((*enemy_it)->get_bounds()))
                {
                    if ((*enemy_it)->take_damage(1))  // Enemy destroyed
                    {
						score_ += (*enemy_it)->get_score_value();
						enemy_it = enemies.erase(enemy_it);

                        if (type == "scout") total_scout_enemies_--;
                        else if (type == "warrior") total_warrior_enemies_--;
                    }
                    else
                    {
                        ++enemy_it;
                    }
                    hit = true;
                    break;
                }
                else
                {
                    ++enemy_it;
                }
            }
        }
        if (hit)
        {
            bullet_it = bullets_.erase(bullet_it);
        }
        else
        {
            ++bullet_it;
        }
    }

	// Bullet-player collisions (enemy bullets)
	for (auto enemy_bullet_it = enemy_bullets_.begin(); enemy_bullet_it != enemy_bullets_.end(); )
    {
        if ((*enemy_bullet_it)->get_bounds().findIntersection(player_.get_bounds()))
        {
            const auto old_hp = player_.get_hp();
            const auto is_player_dead = player_.take_damage(1);
            const auto new_hp = player_.get_hp();
            if (new_hp < old_hp && new_hp >= 0)
            {
                health_icons_[new_hp]->setTexture(ui_textures_["heart_empty"]);
            }
            if (is_player_dead)
            {
                // Do not continue checking if player is dead
                break;
            }
            else
            {
                enemy_bullet_it = enemy_bullets_.erase(enemy_bullet_it);
            }
        }
        else
        {
            ++enemy_bullet_it;
        }
	}

}//!update
//---------------------------------------------------------------------------------------

void Game_scene::draw(sf::RenderTarget& render_target)
{
    // UI
    for (const auto& sky : sky_sprites_) { render_target.draw(*sky); }
    render_target.draw(*controls_);
    for (const auto& icon : health_icons_) { render_target.draw(*icon); }
    win_cond_label_.draw(render_target);

    // Game objects
    draw_game_objects(render_target);

	//Pause overlay
    if (paused_)
    {
        render_target.draw(pause_overlay_);
        pause_panel_->draw(render_target);
        for (auto& [_, btn] : pause_buttons_) btn->draw(render_target);
    }
}//!draw
//---------------------------------------------------------------------------------------

void Game_scene::draw_game_objects(sf::RenderTarget& render_target)
{
    player_.draw(render_target);
    flev::debug::draw_debug_bounds(render_target, player_.get_bounds());
    for (const auto& bullet : bullets_)
    {
        bullet->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, bullet->get_bounds());
    }
    
	// 1st level enemies
    for (const auto& enemy : enemies_["big_stone"])
    {
        enemy->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, enemy->get_bounds());
    }
    for (const auto& enemy : enemies_["small_stone"])
    {
        enemy->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, enemy->get_bounds());
    }

    // 2nd level enemies
    for (const auto& enemy : enemies_["warrior"])
    {
        enemy->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, enemy->get_bounds());
    }
    for (const auto& enemy : enemies_["scout"])
    {
        enemy->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, enemy->get_bounds());
    }
	// Enemy bullets
    for (const auto& bullet : enemy_bullets_)
    {
        bullet->draw(render_target);
        flev::debug::draw_debug_bounds(render_target, bullet->get_bounds());
	}
}//!draw_game_objects
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Game_scene::get_scene_type() const
{ 
    return Game_state::Game; 
}//!get_scene_type
//---------------------------------------------------------------------------------------

void Game_scene::initialize_sky(const sf::Vector2u& window_size)
{
    switch (current_level_id_)
    {
    case 0:
    {
        // Load sky texture
        if (!ui_textures_.contains("level_0_bg") &&
            !ui_textures_["level_0_bg"].loadFromFile("assets/level_0_bg.png"))
        {
			LOG_ERROR(get_global_logger(), "Failed to load level '{}' background.", current_level_id_);
            return;
        }
        sf::Texture& sky_texture = ui_textures_["level_0_bg"];

        // Sky
        sky_sprites_.emplace_back(std::make_unique<sf::Sprite>(sky_texture));
        sky_sprites_.emplace_back(std::make_unique<sf::Sprite>(sky_texture));

        float scale_x = static_cast<float>(window_size.x) / sky_texture.getSize().x;
        float scale_y = static_cast<float>(window_size.y) / sky_texture.getSize().y;

        sky_sprites_[0]->setScale({ scale_x, scale_y });
        sky_sprites_[1]->setScale({ scale_x, scale_y });
        sky_sprites_[1]->setPosition({ sky_sprites_[0]->getGlobalBounds().size.x, 0 });

        break;
    }
    case 1:
    {
        // Load sky texture
        if (!ui_textures_.contains("level_1_bg") &&
            !ui_textures_["level_1_bg"].loadFromFile("assets/level_1_bg.jpg"))
        {
            LOG_ERROR(get_global_logger(), "Failed to load level '{}' background.", current_level_id_);
            return;
        }
        sf::Texture& sky_texture = ui_textures_["level_1_bg"];

        // Sky
        sky_sprites_.emplace_back(std::make_unique<sf::Sprite>(sky_texture));

        float scale_x = static_cast<float>(window_size.x) / sky_texture.getSize().x;
        float scale_y = static_cast<float>(window_size.y) / sky_texture.getSize().y;

        sky_sprites_[0]->setScale({ scale_x, scale_y });
        break;
    }
    default:
        break;
    }
}//!initialize_sky
//---------------------------------------------------------------------------------------

void Game_scene::initialize_ui(const sf::Vector2u& window_size)
{
    // Controls
    if (!ui_textures_.contains("controls") &&
        !ui_textures_["controls"].loadFromFile("assets/controls.png"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load controls texture.");
    }
    auto& controls_texture = ui_textures_["controls"];
	controls_ = std::make_unique<sf::Sprite>(controls_texture);
    controls_->setScale({ 0.2f,  0.2f });
    controls_->setPosition({
        static_cast<float>(window_size.x) - controls_->getGlobalBounds().size.x,
        static_cast<float>(window_size.y) - controls_->getGlobalBounds().size.y
    });

    // Health icons
    if (!ui_textures_.contains("heart_full") &&
        !ui_textures_["heart_full"].loadFromFile("assets/heart_full.png"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load full health icon texture.");
    }
    if (!ui_textures_.contains("heart_empty") &&
        !ui_textures_["heart_empty"].loadFromFile("assets/heart_empty.png"))
    {
        LOG_ERROR(get_global_logger(), "Failed to load empty health icon texture.");
    }
    auto& heart_texture = ui_textures_["heart_full"];
    for (int i = 0; i < player_.get_max_hp(); ++i)
    {
        auto icon = std::make_unique<sf::Sprite>(heart_texture);
        icon->setScale({ 0.1f, 0.1f });
        icon->setPosition({ 20.f + i * (icon->getGlobalBounds().size.x + 10.f), 20.f });
        health_icons_.push_back(std::move(icon));
    }

    // Labels
    if (!ui_font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load UI font.");
    }

    switch (current_level_id_)
    {
    case 0:
    {
        win_cond_label_ = Label(std::format("До станции осталось: {} миль.", level_duration_ / 2), ui_font_, 30);
        win_cond_label_.set_color(sf::Color::White);
        break;
    }
    case 1:
    {
		const auto total_enemies = total_scout_enemies_ + total_warrior_enemies_;
        win_cond_label_ = Label(std::format("Противников осталось: {}", total_enemies), ui_font_, 30);
        win_cond_label_.set_color(sf::Color::White);
        break;
    }
    case 2:
    {
        win_cond_label_ = Label("Вы в зоне повышенной опасности!", ui_font_, 30);
        win_cond_label_.set_color(sf::Color::Red);
        break;
    }
    }
    win_cond_label_.set_position({ 20.f, 120.f });
}//!initialize_ui
//---------------------------------------------------------------------------------------

void Game_scene::initialize_pause_menu(const sf::Vector2u& window_size)
{
    // Shading
    pause_overlay_.setSize(sf::Vector2f(
        static_cast<float>(window_size.x),
        static_cast<float>(window_size.y)
    ));
    pause_overlay_.setFillColor(sf::Color(40, 40, 60, 200));

    // Panel
    pause_panel_ = std::make_unique<Panel>();
    pause_panel_->set_size({ 500.f, 300.f });
    pause_panel_->set_position({
        (window_size.x - 500.f) / 2.f,
        (window_size.y - 300.f) / 2.f
    });

    // Buttons
    pause_buttons_.emplace("resume", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, { 200.f, 50.f }),
        "Продолжить",
        ui_font_,
        30u
    ));
    pause_buttons_.emplace("menu", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, { 200.f, 50.f }),
        "В меню",
        ui_font_,
        30u
    ));
    pause_buttons_.emplace("exit", std::make_unique<Button>(
        sf::FloatRect({ 0, 0 }, { 200.f, 50.f }),
        "Выйти",
        ui_font_,
        30u
    ));

    // Positioning buttons on the panel
    const auto panel_pos = pause_panel_->get_bounds().position;
    const auto panel_center = pause_panel_->get_bounds().getCenter();

    pause_buttons_["resume"]->set_position({ panel_center.x - 100.f, panel_pos.y + 60.f });
    pause_buttons_["menu"]->set_position({ panel_center.x - 100.f, panel_pos.y + 130.f });
    pause_buttons_["exit"]->set_position({ panel_center.x - 100.f, panel_pos.y + 200.f });
}//!initialize_pause_menu
//---------------------------------------------------------------------------------------

void Game_scene::spawn_enemy()
{
	// If level has a duration, do not spawn enemies in the last 5 seconds
    if (level_duration_ > 0 && 
        level_timer_.getElapsedTime().asSeconds() > level_duration_ - 5)
    {
		return;
    }
	static uint8_t wave_number = 0;
    auto win_size = main_window_.get_window_size();

    switch (current_level_id_)
    {
    case 0: // Meteors
    {
        const float x = static_cast<float>(rand() % (win_size.x) + win_size.x / 6);
        if (wave_number++ % 3)
        {
            enemies_["big_stone"].emplace_back(std::make_unique<Big_stone>(sf::Vector2f(x + 150, -100)));
        }
        enemies_["small_stone"].emplace_back(std::make_unique<Small_stone>(sf::Vector2f(x + 50, -100)));
        break;
    }
    case 1: // Ships
    {
        if (wave_number++ % 3 == 0)
        {
			// Spawn scouts (2 per every third wave)
            for (size_t i = 0; i < 2; i++)
            {
                if (enemies_["scout"].size() >= total_scout_enemies_)
                {
                    break;
                }
                const float y = static_cast<float>(rand() % (win_size.y - 100u) + 50.f);
                enemies_["scout"].emplace_back(std::make_unique<Scout>(sf::Vector2f(win_size.x + 50, y)));
            }
        }
        else
        {
            if (enemies_["warrior"].size() >= total_warrior_enemies_)
            {
                break;
            }
            const float y = static_cast<float>(rand() % (win_size.y / 2u));
            enemies_["warrior"].emplace_back(std::make_unique<Warrior>(sf::Vector2f(win_size.x + 50, y)));
        }
        break;
    }
    }
}//!spawn_enemy
//---------------------------------------------------------------------------------------

void Game_scene::update_sky(const float dt)
{
	// Only level 0 has scrolling sky
    if (current_level_id_ != 0) return;

    float scroll_speed_ = player_.get_speed() * 0.4f;
    for (const auto& sky : sky_sprites_)
    {
        sky->move({ -scroll_speed_ * dt, 0 });
    }

    float sky_width = sky_sprites_[0]->getGlobalBounds().size.x;
    if (sky_sprites_[0]->getPosition().x + sky_width < 0)
        sky_sprites_[0]->setPosition({ sky_sprites_[1]->getPosition().x + sky_width, 0 });
    if (sky_sprites_[1]->getPosition().x + sky_width < 0)
        sky_sprites_[1]->setPosition({ sky_sprites_[0]->getPosition().x + sky_width, 0 });

}//!update_sky
//---------------------------------------------------------------------------------------