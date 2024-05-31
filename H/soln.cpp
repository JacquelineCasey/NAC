// https://open.kattis.com/problems/mountaincraft

#include <iostream>
#include <iomanip>
#include <limits>
#include <numbers>
#include <set>
#include <cmath>

class SegmentTree {
    /* A segment tree storing intervals. Admits two operations,
     * insert (for turning on an interval), and remove (for turning off an interval).
     * We use laziness to elide the need to create every theoretical segtree node,
     * allowing us to operate over a very large (~10^9) range. */
public: 
    /* The left bound (inclusive). */
    long long left_bound;
    /* The right bound (inclusive). */
    long long right_bound;  
    /* Number of intervals covering the whole range [left, right],
     * exludes those that fully cover the parent */
    int interval_count {0};  
    /* Number of covered cells. Calculated to be either right - left + 1 (if interval_count >= 0)
     * or defers to child nodes. */
    long long covered_cell_count {0};
    /* Left child (if exists) */
    SegmentTree* left_child {nullptr};  
    /* Right child (if exists). Exists if and only if left child exists. */
    SegmentTree* right_child {nullptr};                                  

    SegmentTree(long long left_bound, long long right_bound)
        : left_bound {left_bound}, right_bound {right_bound}
    {}

    /* Helper function that ensures children are added to the current node. */
    void add_children_as_needed() {
        if (left_child != nullptr) {
            return;
        }

        long long mid = (left_bound + right_bound) / 2;
        left_child = new SegmentTree(left_bound, mid);
        right_child = new SegmentTree(mid + 1, right_bound);
    }

    /* Helper function that has a node recompute its covered_cell_count value.
     * Assumes that children have accurate values of covered_cell_count */
    void recalculate_coverage() {
        /* An active interval fully covers the region */
        if (interval_count > 0) {
            covered_cell_count = right_bound - left_bound + 1;
            return;
        }

        /* Check on coverage in children. No children means no coverage. */
        if (left_child == nullptr) {
            covered_cell_count = 0;
            return;
        }

        /* If children exist, then we ask them for coverage. */
        covered_cell_count = left_child->covered_cell_count + right_child->covered_cell_count;
    }
    
    /* Inserts / Deletes an interval. The total coverage for the tree is placed in the root. 
     * The interval is given as [interval_left, interval_right] (inclusive).
     * val is +1 for insert, -1 for remove. */
    void update(long long interval_left, long long interval_right, int val) {
        // Case 1: Interval is not related to node.
        if (interval_right < left_bound || interval_left > right_bound) {
            return;
        }

        // Case 2: Interval covers node.
        if (interval_left <= left_bound && interval_right >= right_bound) {
            interval_count += val;
            recalculate_coverage();
            return;
        }

        // Case 3: Interval partially overlaps node.
        add_children_as_needed();
        left_child->update(interval_left, interval_right, val);
        right_child->update(interval_left, interval_right, val);
        recalculate_coverage();
    }
};

int main() { 
    int q;
    long long w;

    std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
    std::cin >> q >> w;

    SegmentTree T(0, w - 1); 
    std::set<std::pair<int, int>> mountains {};

    for (int i {0}; i < q; i++) {
        long long x;
        long long y;

        std::cin >> x >> y;

        int left = x - y;
        int right = x + y - 1;

        if (mountains.contains({x, y})) {
            T.update(left, right, -1);
            mountains.erase({x, y});
        }
        else {
            T.update(left, right, 1);
            mountains.insert({x, y});
        }

        std::cout << T.covered_cell_count * std::sqrt(2) << '\n';
    }
}
