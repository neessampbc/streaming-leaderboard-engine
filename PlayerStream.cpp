/**
 * @file PlayerStream.cpp
 * @author Neessam Moustafa
 * @brief Implementation of VectorPlayerStream for CSCI 33500 Project 3.
 * @date 2026-05-05
 */

#include "PlayerStream.hpp"

// ---- VectorPlayerStream -------------------------------------------------

VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : data_(players), index_(0) {}

Player VectorPlayerStream::nextPlayer() {
    if (index_ >= data_.size()) {
        throw std::runtime_error("VectorPlayerStream: no more players remaining.");
    }
    return data_[index_++];
}

size_t VectorPlayerStream::remaining() const {
    return data_.size() - index_;
}
