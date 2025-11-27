#include "GreedyCoinChangeProblem.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace {

using Stock = GreedyCoinChangeProblem::Stock;
using Used = GreedyCoinChangeProblem::Used;
using Result = GreedyCoinChangeProblem::Result;

struct Summary {
    unsigned trials = 0;
    unsigned unreachableMismatch = 0;   // Greedy said unreachable but optimal exists (or vice versa).
    unsigned coinCountMismatch = 0;     // Both reachable but coin counts differ.
    unsigned invalidSum = 0;            // Greedy claims success but sum != target.
};

unsigned toUnsigned(const std::string &value, unsigned defaultValue) {
    try {
        return value.empty() ? defaultValue : static_cast<unsigned>(std::stoul(value));
    } catch (...) {
        return defaultValue;
    }
}

Stock randomStock(std::mt19937_64 &rng, unsigned maxCount) {
    std::uniform_int_distribution<unsigned> countDist(0, maxCount);
    Stock stock;
    for (auto denom : GreedyCoinChangeProblem::kStandardDenominations) {
        stock[denom] = countDist(rng);
    }
    return stock;
}

std::optional<unsigned> boundedOptimalCoins(const Stock &stock, unsigned target) {
    const unsigned inf = std::numeric_limits<unsigned>::max() / 4;
    std::vector<unsigned> dp(target + 1, inf);
    dp[0] = 0;

    for (auto denom : GreedyCoinChangeProblem::kStandardDenominations) {
        unsigned remaining = stock.at(denom);
        unsigned batch = 1;
        while (remaining > 0) {
            unsigned take = std::min(batch, remaining);
            unsigned value = take * denom;
            for (unsigned v = target + 1; v-- > value;) {
                if (dp[v - value] != inf) {
                    dp[v] = std::min(dp[v], dp[v - value] + take);
                }
            }
            remaining -= take;
            batch <<= 1;
        }
    }

    if (dp[target] == inf) {
        return std::nullopt;
    }
    return dp[target];
}

struct GreedyEvaluation {
    bool reachable = false;
    unsigned coinCount = 0;
    unsigned sum = 0;
};

GreedyEvaluation evaluateGreedy(const Result &result) {
    if (!result.has_value()) {
        return {};
    }

    GreedyEvaluation eval;
    eval.reachable = true;
    eval.coinCount = 0;
    eval.sum = 0;
    for (auto [denom, count] : result.value()) {
        eval.coinCount += count;
        eval.sum += denom * count;
    }
    return eval;
}

} // namespace

int main(int argc, char **argv) {
    const unsigned defaultTrials = 5000;
    const unsigned defaultMaxCount = 8;
    const unsigned defaultTarget = 20000;

    const unsigned trials = (argc > 1) ? toUnsigned(argv[1], defaultTrials) : defaultTrials;
    const unsigned maxCount = (argc > 2) ? toUnsigned(argv[2], defaultMaxCount) : defaultMaxCount;
    const unsigned maxTarget = (argc > 3) ? toUnsigned(argv[3], defaultTarget) : defaultTarget;

    GreedyCoinChangeProblem solver;
    Summary summary;

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<unsigned> targetDist(0, maxTarget);

    for (unsigned i = 0; i < trials; ++i) {
        Stock stock = randomStock(rng, maxCount);
        unsigned target = targetDist(rng);

        Result greedyResult = solver.Excute(stock, static_cast<int>(target));
        auto optimal = boundedOptimalCoins(stock, target);
        GreedyEvaluation eval = evaluateGreedy(greedyResult);

        ++summary.trials;

        if (eval.reachable != optimal.has_value()) {
            ++summary.unreachableMismatch;
            continue;
        }

        if (!eval.reachable) {
            continue; // Both agree unreachable.
        }

        if (eval.sum != target) {
            ++summary.invalidSum;
            continue;
        }

        if (eval.coinCount != *optimal) {
            ++summary.coinCountMismatch;
        }
    }

    const auto pct = [](unsigned part, unsigned total) -> double {
        return total == 0 ? 0.0 : (static_cast<double>(part) * 100.0) / static_cast<double>(total);
    };

    std::cout << "Trials: " << summary.trials << '\n';
    std::cout << "Reachability mismatches: " << summary.unreachableMismatch
              << " (" << pct(summary.unreachableMismatch, summary.trials) << "%)\n";
    std::cout << "Sum mismatches: " << summary.invalidSum
              << " (" << pct(summary.invalidSum, summary.trials) << "%)\n";
    std::cout << "Coin-count mismatches: " << summary.coinCountMismatch
              << " (" << pct(summary.coinCountMismatch, summary.trials) << "%)\n";

    if (summary.unreachableMismatch == 0 && summary.invalidSum == 0 && summary.coinCountMismatch == 0) {
        std::cout << "Greedy implementation matched bounded optimal results in all trials.\n";
        return 0;
    }

    std::cout << "Discrepancies detected; investigate greedy logic or stock assumptions.\n";
    return 1;
}
