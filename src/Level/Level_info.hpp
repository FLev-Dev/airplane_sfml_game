#pragma once 
#include <ctypes>

struct Level_info
{
    int32_t id = 0;
    std::string name;

    std::string background_path;
	bool is_background_static = true;
	
    float duration = 0.f; // 0 means infinite (seconds)
    bool is_unlocked = false;
};