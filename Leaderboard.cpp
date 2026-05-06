/**
 * @file Leaderboard.cpp
 * @author Neessam Moustafa
 * @brief Implementation of heapRank, quickSelectRank, and rankIncoming
 *        for CSCI 33500 Project 3.
 * @date 2026-05-05
 */

#include "Leaderboard.hpp"
#include <algorithm>
#include <chrono>
#include <functional>   // std::greater
#include <cstddef>

// ================================================================
//  Task 1A: heapRank
//
//  Early-stopping heapsort using std::make_heap / std::pop_heap.
//
//  After k = floor(n/10) calls to pop_heap, the vector looks like:
//
//    [ ... unsorted heap ... | k-th max | (k-1)-th max | ... | 1st max ]
//                              ^-- index n-k              ^-- index n-1
//
//  That tail segment [n-k, n-1] is already in ascending order
//  (k-th largest at n-k, absolute maximum at n-1).
// ================================================================

RankingResult heapRank(std::vector<Player>& players) {
    using Clock = std::chrono::high_resolution_clock;
    using Ms    = std::chrono::duration<double, std::milli>;

    auto start = Clock::now();

    RankingResult result;
    const size_t n = players.size();
    const size_t k = n / 10;   // floor(10%)

    if (k > 0) {
        // Build a max-heap in-place — O(n).
        std::make_heap(players.begin(), players.end());

        // Extract the k largest elements one by one.
        // pop_heap(first, last) moves *first (current max) to *(last-1),
        // then re-heapifies [first, last-1).  We shrink the "last" boundary
        // by i on each call so already-placed elements are not disturbed.
        for (size_t i = 0; i < k; ++i) {
            std::pop_heap(players.begin(),
                          players.end() - static_cast<std::ptrdiff_t>(i));
        }

        // players[n-k .. n-1] now holds the top-k in ascending order.
        result.top_.assign(players.begin() + static_cast<std::ptrdiff_t>(n - k),
                           players.end());
    }

    result.elapsed_ = Ms(Clock::now() - start).count();
    return result;
}

// ================================================================
//  Task 1B: quickSelectRank
//
//  Helper functions live in an anonymous namespace so they are
//  private to this translation unit and do not pollute the global
//  namespace.
// ================================================================

namespace {

/**
 * @brief Partitions players[left..right] using a median-of-three pivot.
 *
 * Sorts players[left], players[mid], players[right] in place, then
 * places the median at players[right] as the Lomuto pivot.  After the
 * call:
 *   players[left .. pivotIdx-1]  < pivot
 *   players[pivotIdx]           == pivot
 *   players[pivotIdx+1 .. right] >= pivot
 *
 * @param players The vector being sorted.
 * @param left    Leftmost index of the subrange (inclusive).
 * @param right   Rightmost index of the subrange (inclusive).
 * @return The final index of the pivot element.
 */
int partition(std::vector<Player>& players, int left, int right) {
    if (left >= right) return left;

    // Two-element subarray: just sort in place and return.
    if (right - left == 1) {
        if (players[right] < players[left])
            std::swap(players[left], players[right]);
        return left;
    }

    // Median-of-three: sort players[left], players[mid], players[right]
    // so that players[left] <= players[mid] <= players[right].
    int mid = left + (right - left) / 2;
    if (players[mid]   < players[left])  std::swap(players[mid],  players[left]);
    if (players[right] < players[left])  std::swap(players[right], players[left]);
    if (players[right] < players[mid])   std::swap(players[right], players[mid]);

    // Place the median at the end to serve as the Lomuto pivot.
    std::swap(players[mid], players[right]);
    const Player pivot = players[right];

    // Standard Lomuto partition.
    int i = left - 1;
    for (int j = left; j < right; ++j) {
        if (players[j] < pivot) {
            ++i;
            std::swap(players[i], players[j]);
        }
    }
    std::swap(players[i + 1], players[right]);
    return i + 1;
}

/**
 * @brief Quickselect: rearranges players[left..right] so that
 *        players[targetIdx] holds the element that would appear at
 *        that position in a fully sorted (ascending) array.
 *
 * @pre  left <= targetIdx <= right
 * @param players   The vector being partitioned.
 * @param left      Leftmost index of the current subrange.
 * @param right     Rightmost index of the current subrange.
 * @param targetIdx The index whose correct element we are finding.
 * @post players[targetIdx] == k-th order statistic;
 *       [left, targetIdx-1] <= players[targetIdx] <= [targetIdx+1, right].
 */
void quickSelectHelper(std::vector<Player>& players,
                       int left, int right, int targetIdx) {
    if (left >= right) return;

    int pivotIdx = partition(players, left, right);

    if      (pivotIdx == targetIdx) return;
    else if (pivotIdx <  targetIdx)
        quickSelectHelper(players, pivotIdx + 1, right,       targetIdx);
    else
        quickSelectHelper(players, left,          pivotIdx - 1, targetIdx);
}

/**
 * @brief Recursive quicksort over players[left..right] (ascending).
 *
 * @param players The vector to sort.
 * @param left    Leftmost index of the subrange.
 * @param right   Rightmost index of the subrange.
 * @post players[left..right] is sorted in ascending order.
 */
void quickSortHelper(std::vector<Player>& players, int left, int right) {
    if (left >= right) return;
    int pivotIdx = partition(players, left, right);
    quickSortHelper(players, left,          pivotIdx - 1);
    quickSortHelper(players, pivotIdx + 1, right);
}

}  // anonymous namespace

RankingResult quickSelectRank(std::vector<Player>& players) {
    using Clock = std::chrono::high_resolution_clock;
    using Ms    = std::chrono::duration<double, std::milli>;

    auto start = Clock::now();

    RankingResult result;
    const int n = static_cast<int>(players.size());
    const int k = n / 10;   // floor(10%)

    if (k > 0) {
        // targetIdx: 0-based position of the k-th largest element in sorted order.
        // After quickselect, everything in [targetIdx, n-1] belongs to the top-k.
        const int targetIdx = n - k;

        quickSelectHelper(players, 0, n - 1, targetIdx);
        quickSortHelper(players, targetIdx, n - 1);

        result.top_.assign(players.begin() + targetIdx, players.end());
    }

    result.elapsed_ = Ms(Clock::now() - start).count();
    return result;
}

// ================================================================
//  Task 2C: rankIncoming
//
//  Online algorithm using a min-heap of fixed size reporting_interval.
//
//  Phase 1 (fill): accumulate players until the heap reaches capacity,
//                  then call make_heap once.
//  Phase 2 (stream): for each subsequent player, call replaceMin() if
//                    the new player beats the current heap minimum.
//
//  Cutoffs recorded:
//    - At every multiple of reporting_interval (once heap is formed).
//    - At the very end regardless of alignment.
//
//  Stream fetch (nextPlayer) is excluded from elapsed_ timing.
// ================================================================

RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval) {
    using Clock = std::chrono::high_resolution_clock;
    using Ms    = std::chrono::duration<double, std::milli>;

    RankingResult result;
    if (reporting_interval == 0) return result;

    std::vector<Player> heap;
    heap.reserve(reporting_interval);

    size_t totalRead  = 0;
    double elapsed    = 0.0;
    bool   heapFormed = false;  // true once make_heap has been called

    while (stream.remaining() > 0) {
        // Fetch next player — NOT timed per spec.
        Player p = stream.nextPlayer();

        // Begin timed section.
        auto t1 = Clock::now();

        if (!heapFormed) {
            // Fill phase: push until we have reporting_interval players.
            heap.push_back(p);

            if (heap.size() == reporting_interval) {
                // Build min-heap: smallest level at front.
                std::make_heap(heap.begin(), heap.end(), std::greater<Player>());
                heapFormed = true;
            }
        } else {
            // Stream phase: evict the minimum if the new player is better.
            if (p > heap.front()) {
                replaceMin(heap.begin(), heap.end(), p);
            }
        }

        ++totalRead;

        // Record milestone cutoff (heap must be fully formed first).
        if (heapFormed && totalRead % reporting_interval == 0) {
            result.cutoffs_[totalRead] = heap.front().getLevel();
        }

        elapsed += Ms(Clock::now() - t1).count();
        // End timed section.
    }

    // Final timed section: form heap if never filled, record last cutoff, sort.
    auto t1 = Clock::now();

    // If stream ended before heap reached capacity, build it now.
    if (!heap.empty() && !heapFormed) {
        std::make_heap(heap.begin(), heap.end(), std::greater<Player>());
    }

    // Always record the final cutoff (spec: "regardless of being a multiple
    // of the reporting interval").
    if (!heap.empty() &&
        result.cutoffs_.find(totalRead) == result.cutoffs_.end()) {
        result.cutoffs_[totalRead] = heap.front().getLevel();
    }

    // Sort heap into ascending order for top_ output.
    std::sort(heap.begin(), heap.end());
    result.top_ = std::move(heap);

    elapsed += Ms(Clock::now() - t1).count();

    result.elapsed_ = elapsed;
    return result;
}
