#include "Leaderboard_scene.hpp"
#include "../Main_window.hpp"
#include "../Leaderboard_entry.hpp"

#include <utils/debug_bounds.hpp>

Leaderboard_scene::Leaderboard_scene(Main_window& window, const std::vector<Leaderboard_entry>& entries)
    : Scene(window), entries_(entries)
{
    const auto window_size = window.get_window_size();

	// Background
    if (!background_tex_.loadFromFile("assets/main_menu_bg.png"))
    {
        LOG_ERROR(get_global_logger(), "Failed to load background texture for leaderboards.");
    }
    background_ = std::make_unique<sf::Sprite>(background_tex_);
    background_->setScale({
        static_cast<float>(window_size.x) / background_tex_.getSize().x,
        static_cast<float>(window_size.y) / background_tex_.getSize().y
    });

    // Overlay
    overlay_.setSize(sf::Vector2f(static_cast<float>(window_size.x), static_cast<float>(window_size.y)));
    overlay_.setFillColor(sf::Color(40, 40, 60, 200));

    // Panel
    panel_ = std::make_unique<Decorated_panel>("assets/panel.png");
    const float panel_w = 600.f, panel_h = 800.f;
    panel_->set_size({ panel_w, panel_h });
    panel_->set_position({ (window_size.x - panel_w) / 2.f, (window_size.y - panel_h) / 2.f });

    // Font
    if (!font_.openFromFile("assets/timesnewromanpsmt.ttf"))
    {
		LOG_ERROR(get_global_logger(), "Failed to load font for leaderboard scene.");
    }

	panel_->set_title("Таблица лидеров", font_, 30);

	// Calculate visible entries
    visible_entries_ = static_cast<int32_t>(panel_h / entry_height_) - 2;
}//!Leaderboard_scene
//---------------------------------------------------------------------------------------

void Leaderboard_scene::handle_event(const sf::Event& event)
{
    if (auto wheel = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (wheel->delta != 0)
        {
            float max_scroll = std::max(0.f, (static_cast<float>(entries_.size()) - visible_entries_) * entry_height_);
            scroll_offset_ -= wheel->delta * 20.f;
            scroll_offset_ = std::clamp(scroll_offset_, 0.f, max_scroll);
			virtual_rows_dirty_ = true;
        }
    }
    else if (auto key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            main_window_.switch_to(Game_state::Main_Menu);
        }
    }
}//!handle_event
//---------------------------------------------------------------------------------------

void Leaderboard_scene::draw(sf::RenderTarget& render_target)
{
    render_target.draw(*background_);
    render_target.draw(overlay_);
    panel_->draw(render_target);

	// Draw entries
    render_entries(render_target);
}//!draw
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Game_state Leaderboard_scene::get_scene_type() const
{ 
    return Game_state::Leaderboard; 
}//!get_scene_type
//---------------------------------------------------------------------------------------

void Leaderboard_scene::render_entries(sf::RenderTarget& render_target)
{
    if (virtual_rows_dirty_)
    {
        virtual_rows_cache_ = build_virtual_rows();
        virtual_rows_dirty_ = false;
    }

    const auto& virtual_rows = virtual_rows_cache_;
    if (virtual_rows.empty()) return;

    const auto panel_bounds = panel_->get_content_bounds();
    const auto start_y = panel_bounds.position.y;

    const auto first_visible = static_cast<uint32_t>(scroll_offset_ / entry_height_);
    const auto last_visible = std::min(
        first_visible + visible_entries_,
        static_cast<uint32_t>(virtual_rows.size())
    );

    for (uint32_t i = first_visible; i < last_visible; ++i)
    {
        const auto& vrow = virtual_rows[i];
        std::string text;

        if (vrow.type == Virtual_row::Type::Player_header)
        {
            const auto& entry = entries_[vrow.player_index];
            int total = 0;
            for (int s : entry.score_per_level_id) total += s;
            text = std::format("{}. {} — Всего: {}",
                vrow.player_index + 1, entry.name, total);
        }
		else // Level_detail
        {
            const auto& entry = entries_[vrow.player_index];
            const auto score = entry.score_per_level_id[vrow.level_id];
            text = std::format("\tУровень {}: {}", vrow.level_id + 1, score);
        }

        Label label(text, font_, vrow.type == Virtual_row::Type::Player_header ? 26 : 22);
        label.set_position({
            panel_bounds.position.x + 30.f,
            start_y + (i - first_visible) * entry_height_ - scroll_offset_ + (first_visible * entry_height_)
        });
        label.set_color(vrow.type == Virtual_row::Type::Player_header ? sf::Color::White : sf::Color::Cyan);

		// Clipping
        if (label.get_bounds().position.x < panel_bounds.position.x + panel_bounds.size.x &&
            label.get_bounds().position.y < panel_bounds.position.y + panel_bounds.size.y)
        {
            label.draw(render_target);
        }
    }
}//!render_entries
//---------------------------------------------------------------------------------------

FLEV_NODISCARD std::vector<Leaderboard_scene::Virtual_row> Leaderboard_scene::build_virtual_rows() const
{
    std::vector<Virtual_row> rows;
    for (size_t i = 0; i < entries_.size(); ++i)
    {
        rows.push_back({ Virtual_row::Type::Player_header, i, -1 });

        // Level details
        const auto& entry = entries_[i];
        for (size_t lvl = 0; lvl < entry.score_per_level_id.size(); ++lvl)
        {
            if (entry.score_per_level_id[lvl] > 0) // Only show levels with scores
            {
                rows.push_back({ Virtual_row::Type::Level_detail, i, static_cast<int32_t>(lvl) });
            }
        }
    }
    return rows;
}//!build_virtual_rows
//---------------------------------------------------------------------------------------