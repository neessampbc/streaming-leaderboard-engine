/**
 * @file Player.hpp
 * @author Neessam Moustafa
 * @brief Declaration of the Player class for CSCI 33500 Project 3.
 * @date 2026-05-05
 *
 * Represents a game player with a name and level.
 * All comparison operators are based solely on level.
 */

#pragma once
#include <string>

/**
 * @brief Represents a game player with a name and an integer level.
 *
 * Comparison operators compare players by level only.
 * operator== also checks the name field.
 */
class Player {
public:
    /**
     * @brief Default constructor. Initializes name to "" and level to 0.
     */
    Player();

    /**
     * @brief Constructs a Player with the given name and level.
     * @param name  The player's display name.
     * @param level The player's level (used for all ranking comparisons).
     */
    Player(const std::string& name, int level);

    /**
     * @brief Returns the player's name.
     * @return A const reference to the name string.
     */
    const std::string& getName() const;

    /**
     * @brief Returns the player's level.
     * @return The integer level of this player.
     */
    int getLevel() const;

    /**
     * @brief Less-than comparison by level.
     * @param other The player to compare against.
     * @return true if this player's level is less than other's.
     */
    bool operator< (const Player& other) const;

    /**
     * @brief Greater-than comparison by level.
     * @param other The player to compare against.
     * @return true if this player's level is greater than other's.
     */
    bool operator> (const Player& other) const;

    /**
     * @brief Less-than-or-equal comparison by level.
     * @param other The player to compare against.
     * @return true if this player's level is <= other's.
     */
    bool operator<=(const Player& other) const;

    /**
     * @brief Greater-than-or-equal comparison by level.
     * @param other The player to compare against.
     * @return true if this player's level is >= other's.
     */
    bool operator>=(const Player& other) const;

    /**
     * @brief Equality comparison by both name and level.
     * @param other The player to compare against.
     * @return true if both name and level are identical.
     */
    bool operator==(const Player& other) const;

    /**
     * @brief Inequality comparison by both name and level.
     * @param other The player to compare against.
     * @return true if name or level differ.
     */
    bool operator!=(const Player& other) const;

private:
    std::string name_;  ///< The player's display name.
    int         level_; ///< The player's level, used for all ranking.
};
