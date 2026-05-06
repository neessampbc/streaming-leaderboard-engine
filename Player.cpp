/**
 * @file Player.cpp
 * @author Neessam Moustafa
 * @brief Implementation of the Player class for CSCI 33500 Project 3.
 * @date 2026-05-05
 */

#include "Player.hpp"

// ---- Constructors -------------------------------------------------------

Player::Player() : name_(""), level_(0) {}

Player::Player(const std::string& name, int level) : name_(name), level_(level) {}

// ---- Accessors ----------------------------------------------------------

const std::string& Player::getName()  const { return name_;  }
int               Player::getLevel() const { return level_; }

// ---- Comparison operators (all compare by level) ------------------------

bool Player::operator< (const Player& o) const { return level_ <  o.level_; }
bool Player::operator> (const Player& o) const { return level_ >  o.level_; }
bool Player::operator<=(const Player& o) const { return level_ <= o.level_; }
bool Player::operator>=(const Player& o) const { return level_ >= o.level_; }

// operator== also checks name so two distinct players at the same level
// are not considered identical.
bool Player::operator==(const Player& o) const {
    return level_ == o.level_ && name_ == o.name_;
}
bool Player::operator!=(const Player& o) const { return !(*this == o); }
