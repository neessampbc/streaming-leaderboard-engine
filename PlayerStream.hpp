/**
 * @file PlayerStream.hpp
 * @author Neessam Moustafa
 * @brief Declarations of the PlayerStream interface and VectorPlayerStream
 *        class for CSCI 33500 Project 3.
 * @date 2026-05-05
 *
 * PlayerStream is an abstract interface for streaming Player objects one at
 * a time (online algorithm input).  VectorPlayerStream wraps a std::vector
 * to implement that interface.
 */

#pragma once
#include "Player.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

// ============================================================
// Abstract interface
// ============================================================

/**
 * @brief Abstract base class for a one-at-a-time Player stream.
 *
 * Concrete subclasses provide players from different sources
 * (vectors, files, network, etc.) behind a uniform interface so
 * that online algorithms are source-agnostic.
 */
class PlayerStream {
public:
    /**
     * @brief Retrieves the next Player in the stream.
     * @return The next Player object.
     * @throws std::runtime_error If no players remain.
     */
    virtual Player nextPlayer() = 0;

    /**
     * @brief Returns the number of players remaining in the stream.
     * @return Count of players not yet fetched.
     */
    virtual size_t remaining() const = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~PlayerStream() = default;
};

// ============================================================
// VectorPlayerStream
// ============================================================

/**
 * @brief A PlayerStream backed by a std::vector<Player>.
 *
 * Wraps an existing vector so that online algorithms can consume
 * it through the PlayerStream interface without holding a direct
 * reference to the underlying container.
 *
 * @example
 *   std::vector<Player> v = { Player("Rykard", 23), Player("Malenia", 99) };
 *   VectorPlayerStream stream(v);
 *   stream.remaining()  // -> 2
 *   stream.nextPlayer() // -> Player("Rykard",  23)
 *   stream.nextPlayer() // -> Player("Malenia", 99)
 *   stream.remaining()  // -> 0
 *   stream.nextPlayer() // -> throws std::runtime_error
 */
class VectorPlayerStream : public PlayerStream {
public:
    /**
     * @brief Constructs a VectorPlayerStream from a vector of Players.
     *
     * Initializes the stream with a sequence of Player objects matching
     * the contents of the given vector.  The vector is copied internally.
     *
     * @param players The vector of Player objects to stream.
     */
    explicit VectorPlayerStream(const std::vector<Player>& players);

    /**
     * @brief Retrieves the next Player in the stream.
     *
     * @return The next Player object in the sequence.
     * @post Updates internal state so the following call returns the
     *       Player after the one just returned.
     * @throws std::runtime_error If there are no more players remaining.
     */
    Player nextPlayer() override;

    /**
     * @brief Returns the number of players remaining in the stream.
     * @return Count of players left to be fetched.
     */
    size_t remaining() const override;

private:
    std::vector<Player> data_;  ///< Internal copy of the player sequence.
    size_t              index_; ///< Index of the next player to return.
};
