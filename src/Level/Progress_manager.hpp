#pragma once
#include <utils/defines.hpp>
#include <string>
#include <set>

class Progress_manager
{
public:
	/** 
	 * @brief Unlocks the level with the given ID. 
	 *
	 * @param level_id[in] - ID of the level to unlock.
	 *  
	 * @returns true if the level was successfully unlocked. 
	 */
    FLEV_NODISCARD bool unlock_level(const int32_t level_id);

	/** @returns true if the level with the given ID is unlocked. */
    FLEV_NODISCARD bool is_level_unlocked(const int32_t level_id) const;

	/** @returns The highest unlocked level ID. */
	FLEV_NODISCARD int32_t get_max_unlocked_level() const;

	/** @returns The maximum level ID available in the game. */
	FLEV_NODISCARD int32_t get_max_level() const;

	/**
	 * @brief Saves the current progress to a JSON file.
	 * 
	 * @param path[in][opt] - Path to the JSON file. [Default: "assets/progress.json"]
	 */
    void save_to_file(const std::string& path = "assets/progress.json");

	/**
	 * @brief Loads progress from a JSON file.
	 *
	 * @param path[in][opt] - Path to the JSON file. [Default: "assets/progress.json"]
	 */
    void load_from_file(const std::string& path = "assets/progress.json");

private:
	std::set<int32_t> unlocked_levels_ = { 0 }; ///< Level 0 is always unlocked
	constexpr static int32_t max_level_id_ = 2; ///< Update when new levels are added
};