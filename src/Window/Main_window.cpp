#include "Main_window.hpp"
#include "Scenes/Main_menu.hpp"
#include "Scenes/Game_scene.hpp"
#include "Scenes/Login_scene.hpp"
#include "Scenes/Victory_scene.hpp"
#include "Scenes/Game_over_scene.hpp"
#include "Scenes/Level_selection.hpp"
#include "Scenes/Leaderboard_scene.hpp"
#include "Leaderboard_entry.hpp"


Main_window::Main_window(const sf::Vector2u& window_size)
{
	db = std::make_unique<Database>("game_database.db");
    if (!db->is_open() || !db->execute_query(
        "CREATE TABLE IF NOT EXISTS scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "player_name TEXT NOT NULL, "
        "level_id INTEGER NOT NULL, "
        "score INTEGER NOT NULL, "
        "UNIQUE(player_name, level_id)"
        ");"
    ))
    {
        LOG_ERROR(get_global_logger(), "Failed to create scores table in database.");
        db.reset();
    }

    window_ = sf::RenderWindow(sf::VideoMode(window_size), "Sky Patrol", sf::Style::Default);
    window_.setVerticalSyncEnabled(true);

    progress_manager_.load_from_file();
    current_level_id_ = progress_manager_.get_max_unlocked_level();

    current_state_ = Game_state::Login;
    current_scene_ = std::make_unique<Login_scene>(*this);    
    (void)game_snapshot_.resize(window_size);
}//!Main_window
//---------------------------------------------------------------------------------------

Main_window::~Main_window()
{
    try
    {
        progress_manager_.save_to_file();
    }
    catch (const std::exception& ex) 
    {
        LOG_ERROR(
            get_global_logger(),
            "Exception caught while saving progress on Main_window destruction: {}",
            ex.what()
		);
    }
}//!~Main_window
//---------------------------------------------------------------------------------------

void Main_window::run()
{
    sf::Clock clock;
    while (window_.isOpen() && !should_close_)
    {
        float dt = clock.restart().asSeconds();
		if (dt > 0.1f) dt = 0.1f; // Spikes protection

		// Events
        while (auto event = window_.pollEvent())
        {
            if (event->getIf<sf::Event::Closed>())
            {
                close();
                break;
            }

			// Redirect event to current scene
            current_scene_->handle_event(event.value());
        }

        // Update
        current_scene_->update(dt);

        // Draw
        window_.clear();
        current_scene_->draw(window_);
        window_.display();
    }
}//!run
//---------------------------------------------------------------------------------------

void Main_window::switch_to(const Game_state state)
{
    if (current_state_ == Game_state::Login)
    {
        player_name_ = static_cast<Login_scene*>(current_scene_.get())->get_player_name();
    }
    current_state_ = state;


    if (state == Game_state::Game)
    {
        current_scene_ = std::make_unique<Game_scene>(*this, current_level_id_);
    }
    else if (state == Game_state::Level_Selection)
    {
        current_scene_ = std::make_unique<Level_selection_scene>(*this);
    }
    else if (state == Game_state::Game_over)
    {
        current_scene_ = std::make_unique<Game_over_scene>(*this);
    }
    else if (state == Game_state::Main_Menu)
    {
        current_scene_ = std::make_unique<Main_menu>(*this, player_name_);
    }
    else if (state == Game_state::Leaderboard)
    {
        create_leaderboard_scene();
    }
    else
    {
        LOG_WARNING(
            get_global_logger(),
            "Switching to unhandled game state {}",
            static_cast<std::underlying_type<Game_state>::type>(state)
        );
    }
}//!switch_to
//---------------------------------------------------------------------------------------

void Main_window::switch_to_victory(const int32_t score)
{
    if (db)
    {
        const auto success = db->execute_prepared(
            "INSERT INTO scores (player_name, level_id, score) "
            "VALUES ({}, {}, {}) "
            "ON CONFLICT(player_name, level_id) DO UPDATE SET "
            "score = MAX(scores.score, excluded.score);",
            {
                player_name_,
                current_level_id_,
                score
            }
        );
        if (!success)
        {
            LOG_ERROR(
                get_global_logger(),
                "Failed to save score for player '{}' on level '{}'",
                player_name_,
                current_level_id_
            );
        }
    }
    current_scene_ = std::make_unique<Victory_scene>(*this, current_level_id_, score);
}//!switch_to_victory
//---------------------------------------------------------------------------------------

void Main_window::switch_to_game(const int32_t level_id)
{
    current_level_id_ = level_id;
    switch_to(Game_state::Game);
}//!switch_to_game
//---------------------------------------------------------------------------------------

FLEV_NODISCARD std::string Main_window::get_player_name()
{ 
    return player_name_; 
}//!get_player_name
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Main_window::is_level_unlocked(const int32_t level_id) const
{ 
    return progress_manager_.is_level_unlocked(level_id); 
}//!is_level_unlocked
//---------------------------------------------------------------------------------------

FLEV_NODISCARD int32_t Main_window::get_max_level_id() const
{ 
    return progress_manager_.get_max_level(); 
}//!get_max_level_id
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Main_window::unlock_next_level()
{
    return progress_manager_.unlock_level(current_level_id_ + 1);
}//!unlock_next_level
//---------------------------------------------------------------------------------------

void Main_window::close()
{
    should_close_ = true;
    window_.close();
}//!close
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::RenderWindow& Main_window::get_window()
{ 
    return window_; 
}//!get_window
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::Vector2u Main_window::get_window_size() const
{ 
    return window_.getSize(); 
}//!get_window_size
//---------------------------------------------------------------------------------------

FLEV_NODISCARD sf::RenderTexture& Main_window::get_snapshot_target()
{
	return game_snapshot_;
}//!get_snapshot_target
//---------------------------------------------------------------------------------------

FLEV_NODISCARD const sf::Texture* Main_window::get_game_snapshot() const
{
    return game_snapshot_.getSize().x > 0 ? &game_snapshot_.getTexture() : nullptr;
}//!get_game_snapshot
//---------------------------------------------------------------------------------------

FLEV_NODISCARD void Main_window::create_leaderboard_scene()
{
    if (!db)
    {
        LOG_ERROR(get_global_logger(), "Database not initialized, cannot switch to Leaderboard scene.");
        return;
    }
    if (!db->execute_query("SELECT * FROM scores"))
    {
        LOG_ERROR(get_global_logger(), "Database selection failed, cannot switch to Leaderboard scene.");
        return;
    }
    const auto cols = db->get_cols();
    const auto name_idx = std::distance(
        cols.begin(),
        std::find(cols.begin(), cols.end(), "player_name")
    );
    const auto level_id_idx = std::distance(
        cols.begin(),
        std::find(cols.begin(), cols.end(), "level_id")
    );
    const auto score_idx = std::distance(
        cols.begin(),
        std::find(cols.begin(), cols.end(), "score")
    );
    const auto rows = db->get_rows();
    std::vector<Leaderboard_entry> entries;
    for (const auto& row : rows)
    {
        const std::string player_name = row[name_idx];
        const int32_t level_id = std::stoi(row[level_id_idx]);
        const int32_t score = std::stoi(row[score_idx]);
        auto it = std::find_if(
            entries.begin(),
            entries.end(),
            [&player_name](const Leaderboard_entry& entry) {
                return entry.name == player_name;
            }
        );
        if (it != entries.end())
        {
            if (it->score_per_level_id.size() <= static_cast<size_t>(level_id))
            {
                it->score_per_level_id.resize(level_id + 1, 0);
            }
            it->score_per_level_id[level_id] = score;
        }
        else
        {
            Leaderboard_entry new_entry;
            new_entry.name = player_name;
            if (new_entry.score_per_level_id.size() <= static_cast<size_t>(level_id))
            {
                new_entry.score_per_level_id.resize(level_id + 1, 0);
            }
            new_entry.score_per_level_id[level_id] = score;
            entries.push_back(new_entry);
        }
    }
    std::sort(
        entries.begin(),
        entries.end(),
        [](const Leaderboard_entry& a, const Leaderboard_entry& b) {
            int32_t total_a = 0;
            for (const auto& score : a.score_per_level_id) total_a += score;
            int32_t total_b = 0;
            for (const auto& score : b.score_per_level_id) total_b += score;
            return total_a > total_b;
        }
    );

    if (entries.size() > 10) entries.resize(10); // Keep only top 10 entries
    current_scene_ = std::make_unique<Leaderboard_scene>(*this, entries);
}//!create_leaderboard_scene