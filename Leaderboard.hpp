/**
 * @file Leaderboard.hpp
 * @author Neessam Moustafa
 * @brief Declarations of RankingResult, replaceMin, and the offline/online
 *        ranking functions for CSCI 33500 Project 3.
 * @date 2026-05-05
 *
 * Task 1 (Offline):
 *   heapRank         - early-stopping heapsort, selects and sorts top 10%
 *   quickSelectRank  - quickselect/quicksort hybrid, selects and sorts top 10%
 *
 * Task 2 (Online):
 *   replaceMin       - modified percolateDown for a min-heap (iterator-based)
 *   rankIncoming     - maintains a running top-k over a PlayerStream
 */

#pragma once
#include "Player.hpp"
#include "PlayerStream.hpp"
#include <vector>
#include <map>
#include <algorithm>    // std::iter_swap, std::distance

// ============================================================
// RankingResult
// ============================================================

/**
 * @brief Aggregates the output of any ranking operation.
 *
 *  top_     - Players sorted in ascending order by level.
 *  cutoffs_ - Maps player-count milestones to the minimum level required
 *             to be in the tracked set at that point.  Empty for offline
 *             algorithms; populated for rankIncoming.
 *  elapsed_ - Wall-clock duration of the ranking/selection operation in
 *             milliseconds.  Stream-fetch time is excluded where noted.
 */
struct RankingResult {
    std::vector<Player>   top_;      ///< Top players in ascending level order.
    std::map<size_t, int> cutoffs_;  ///< Milestone -> min level at that point.
    double                elapsed_;  ///< Duration of the operation (ms).

    /// @brief Default constructor — initialises elapsed_ to 0.
    RankingResult() : elapsed_(0.0) {}
};

// ============================================================
// replaceMin  (Task 2B)
// ============================================================

/**
 * @brief Replaces the root (minimum) of a min-heap with newPlayer,
 *        then percolates down to restore the min-heap property.
 *
 * The heap is stored in a contiguous range [begin, end) with 0-based
 * indexing: *begin is the root.
 *   - Left  child of node i: 2*i + 1
 *   - Right child of node i: 2*i + 2
 *
 * Adapted from the textbook's percolateDown(), updated for iterator-based
 * addressing and 0-based root indexing (textbook uses index 1 as root).
 *
 * @tparam Iterator  A random-access iterator over Player objects.
 * @param  begin     Iterator to the root of the heap.
 * @param  end       Past-the-end iterator.
 * @param  newPlayer New value to place at the root before sifting down.
 * @post   The range [begin, end) satisfies the min-heap property.
 */
template<typename Iterator>
void replaceMin(Iterator begin, Iterator end, const Player& newPlayer) {
    // Overwrite the root (current minimum) with the incoming player.
    *begin = newPlayer;

    auto size = std::distance(begin, end);
    using diff_t = decltype(size);

    diff_t hole = 0;  // index currently being sifted down

    while (true) {
        diff_t left     = 2 * hole + 1;
        diff_t right    = 2 * hole + 2;
        diff_t smallest = hole;

        // Find the smaller of the two children (if they exist).
        if (left  < size && *(begin + left)  < *(begin + smallest))
            smallest = left;
        if (right < size && *(begin + right) < *(begin + smallest))
            smallest = right;

        if (smallest == hole) break;  // heap property satisfied; stop

        std::iter_swap(begin + hole, begin + smallest);
        hole = smallest;
    }
}

// ============================================================
// Task 1: Offline algorithms
// ============================================================

/**
 * @brief Uses an early-stopping version of heapsort to select and sort
 *        the top 10% of players in-place.
 *
 * Builds a max-heap over the entire vector in O(n), then repeatedly
 * calls std::pop_heap to extract the maximum, stopping after exactly
 * k = floor(n / 10) extractions.  After k pops, players[n-k .. n-1]
 * holds the top-k in ascending order and is copied into top_.
 *
 * Auxiliary space complexity: O(1)  (excludes input vector and result)
 *
 * @param players A reference to the vector of Player objects to rank.
 * @return A RankingResult whose:
 *         - top_     contains the top 10% in ascending order.
 *         - cutoffs_ is empty.
 *         - elapsed_ contains the duration (ms) of the operation.
 * @post The order of the parameter vector is modified.
 */
RankingResult heapRank(std::vector<Player>& players);

/**
 * @brief Uses a quickselect/quicksort hybrid to select and sort the top
 *        10% of players in-place.
 *
 * Quickselect (with median-of-three pivot) partitions the vector so that
 * players[n-k] is the k-th largest element and [n-k, n-1] contains the
 * top k = floor(n / 10) players in arbitrary order.  Quicksort then sorts
 * that subrange in ascending order.
 *
 * Auxiliary space complexity: O(log n) average (recursive call stack)
 *
 * @param players A reference to the vector of Player objects to rank.
 * @return A RankingResult whose:
 *         - top_     contains the top 10% in ascending order.
 *         - cutoffs_ is empty.
 *         - elapsed_ contains the duration (ms) of the operation.
 * @post The order of the parameter vector is modified.
 */
RankingResult quickSelectRank(std::vector<Player>& players);

// ============================================================
// Task 2: Online algorithm
// ============================================================

/**
 * @brief Exhausts a PlayerStream, maintaining a running collection of
 *        the top <reporting_interval> highest-leveled players seen so far.
 *
 * Uses a min-heap of fixed capacity reporting_interval (built with
 * std::make_heap and std::greater<Player> for smallest-at-front ordering).
 * When a new player's level exceeds the current heap minimum, replaceMin()
 * swaps them in and restores the heap property.
 *
 * Cutoff levels are recorded:
 *   - Every time totalRead is a multiple of reporting_interval (after the
 *     heap is fully formed).
 *   - Once more after the very last player is read, regardless of alignment.
 *
 * Timing excludes stream.nextPlayer() calls per the spec.
 *
 * @note Do NOT use std::priority_queue.  Use a plain std::vector, the STL
 *       heap operations from <algorithm>, and replaceMin().
 *
 * @param stream              A stream providing Player objects.
 * @param reporting_interval  Heap capacity and milestone frequency.
 * @return A RankingResult whose:
 *         - top_     contains the top <reporting_interval> Players read
 *                    from the stream in sorted (ascending) order.
 *         - cutoffs_ maps player-count milestones to the minimum level
 *                    required at that point, including after ALL players
 *                    have been read regardless of interval alignment.
 *         - elapsed_ contains the duration (ms) of the selection/sorting
 *                    operation, excluding stream fetch time.
 * @post All elements of the stream are consumed (remaining() == 0).
 */
RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval);
