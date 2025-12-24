#pragma once
#include <string>
#include <vector>
#include <cstdint>

/** @brief Represents a single entry in the leaderboard. */
struct Leaderboard_entry
{
    std::string name;
    std::vector<int32_t> score_per_level_id;
};