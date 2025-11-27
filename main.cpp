#include "GreedyCoinChangeProblem.h"
#include <iostream>
#include <numeric>

int main() {
    GreedyCoinChangeProblem solver;
    GreedyCoinChangeProblem::Stock stock = {
            {10000, 1},
            {5000,  1},
            {1000,  1},
            {500,   1},
            {100,   2},
            {50,    2},
            {10,    5},
            {5,     3},
            {1,     10}
    };

    constexpr int target = 186;
    auto result = solver.Excute(stock, target);
    if (!result) {
        std::cerr << "Failed to make change: " << result.error() << '\n';
        return 1;
    }

    const auto &used = *result;
    unsigned total = 0;
    unsigned coins = 0;
    for (auto [denom, count]: used) {
        total += denom * count;
        coins += count;
    }

    std::cout << "Target " << target << " reached using " << coins << " coins.\n";
    std::cout << "Breakdown:\n";
    for (auto [denom, count]: used) {
        if (count == 0) continue;
        std::cout << "  " << denom << " x " << count << '\n';
    }
    std::cout << "Total validated: " << total << '\n';

    return 0;
}
