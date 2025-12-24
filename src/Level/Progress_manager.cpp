#include "Progress_manager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

FLEV_NODISCARD bool Progress_manager::unlock_level(const int32_t level_id)
{
	if (level_id > max_level_id_) return false;
	unlocked_levels_.insert(level_id);
	return true;
}//!unlock_level
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Progress_manager::is_level_unlocked(const int32_t level_id) const
{
	return unlocked_levels_.find(level_id) != unlocked_levels_.end();
}//!is_level_unlocked
//---------------------------------------------------------------------------------------

void Progress_manager::save_to_file(const std::string& path)
{
	nlohmann::json j;
	j["unlocked_levels"] = unlocked_levels_;
	std::ofstream file(path);
	if (file.is_open())
	{
		file << j.dump(4);
		file.close();
	}
}//!save_to_file
//---------------------------------------------------------------------------------------

void Progress_manager::load_from_file(const std::string& path)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		nlohmann::json j;
		file >> j;
		file.close();
		if (j.contains("unlocked_levels"))
		{
			unlocked_levels_.clear();
			for (const auto& level_id : j["unlocked_levels"])
			{
				unlocked_levels_.insert(level_id.get<int32_t>());
			}
		}
	}
}//!load_from_file
//---------------------------------------------------------------------------------------

FLEV_NODISCARD int32_t Progress_manager::get_max_unlocked_level() const
{
	if (unlocked_levels_.empty()) return -1;
	return *unlocked_levels_.rbegin();
}//!get_max_unlocked_level
//---------------------------------------------------------------------------------------

FLEV_NODISCARD int32_t Progress_manager::get_max_level() const
{
	return max_level_id_;
}//!get_max_level
//---------------------------------------------------------------------------------------