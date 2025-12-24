#pragma once
#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <cassert>

/** @brief Combat-capable entity with health points (HP). */
class Unit : public Entity
{
public:
    Unit(const std::string& texture_path, uint32_t max_hp)
        : Entity(texture_path)
        , max_hp_(max_hp)
        , current_hp_(max_hp)
    {
        assert(max_hp > 0 && "Max HP must be positive");
    }//!Unit

    /** @brief Applies damage and returns true if unit is destroyed. */
    bool take_damage(uint32_t damage)
    {
        if (damage <= 0) return false;
        int64_t current_hp = current_hp_;
		current_hp -= static_cast<int64_t>(damage);
        if (current_hp <= 0u)
        {
            current_hp_ = 0u;
            return true; // Destroyed
        }
		current_hp_ = static_cast<uint32_t>(current_hp);
        return false;
    }//!take_damage

    /** @brief Heals the unit (capped at max_hp). */
    void heal(uint32_t amount)
    {
        if (amount <= 0) return;
        current_hp_ = std::min(current_hp_ + amount, max_hp_);
    }//!heal

    /** @brief Returns current HP. */
    FLEV_NODISCARD uint32_t get_hp() const { return current_hp_; }

    /** @brief Returns maximum HP. */
    FLEV_NODISCARD uint32_t get_max_hp() const { return max_hp_; }

    /** @brief Checks if unit is alive (HP > 0). */
    FLEV_NODISCARD bool is_alive() const { return current_hp_ > 0u; }

protected:
	uint32_t max_hp_ = 1u;      ///< Maximum health points.
	uint32_t current_hp_ = 1u;  ///< Current health points.
};