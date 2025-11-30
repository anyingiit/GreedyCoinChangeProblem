#include "GreedyCoinChangeProblemImprovedVersion.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace {

    using DenominationTable = GreedyCoinChangeProblemImprovedVersion::DenominationTable;
    using Used = GreedyCoinChangeProblemImprovedVersion::Used;
    using Result = GreedyCoinChangeProblemImprovedVersion::Result;

    struct Summary {
        unsigned trials = 0;
        unsigned feasibilityMismatch = 0;   // Greedy success differs from exhaustive feasibility.
        unsigned sumMismatch = 0;           // Greedy claims success but total != target.
        unsigned constraintMismatch = 0;    // Greedy claims success but violates depth constraint.
        unsigned optimalityMismatch = 0;    // Greedy feasible but uses more coins than optimal.
    };

    struct TableData {
        std::vector<unsigned> denom;  // Sorted from large to small.
        std::vector<unsigned> stock;
        std::vector<unsigned> depth;
    };

    unsigned toUnsigned(const std::string &value, unsigned defaultValue) {
        try {
            return value.empty() ? defaultValue : static_cast<unsigned>(std::stoul(value));
        } catch (...) {
            return defaultValue;
        }
    }

    TableData toTableData(const DenominationTable &table) {
        TableData data;
        data.denom.reserve(table.size());
        data.stock.reserve(table.size());
        data.depth.reserve(table.size());

        for (auto it = table.rbegin(); it != table.rend(); ++it) { // map is ascending; walk descending.
            data.denom.push_back(it->first);
            data.stock.push_back(it->second.stock);
            data.depth.push_back(it->second.maxFallbackDepth);
        }
        return data;
    }

    DenominationTable randomTable(std::mt19937_64 &rng, unsigned maxStock, unsigned maxChainLen) {
        std::uniform_int_distribution<unsigned> lenDist(3, std::max(3u, maxChainLen));
        std::uniform_int_distribution<unsigned> unitDist(1, 20);
        std::uniform_int_distribution<unsigned> factorDist(2, 6);
        std::uniform_int_distribution<unsigned> stockDist(0, maxStock);

        const unsigned length = lenDist(rng);

        std::vector<unsigned> chain(length);
        chain[length - 1] = unitDist(rng);
        for (int i = static_cast<int>(length) - 2; i >= 0; --i) {
            chain[i] = chain[i + 1] * factorDist(rng);
        }

        DenominationTable table;
        for (unsigned i = 0; i < length; ++i) {
            unsigned depthMax = static_cast<unsigned>(length - 1 - i);
            std::uniform_int_distribution<unsigned> depthDist(0, depthMax);
            table[chain[i]] = {stockDist(rng), depthDist(rng)};
        }
        return table;
    }

    unsigned long long totalValue(const TableData &data) {
        unsigned long long total = 0;
        for (std::size_t i = 0; i < data.denom.size(); ++i) {
            total += static_cast<unsigned long long>(data.denom[i]) * data.stock[i];
        }
        return total;
    }

    unsigned pickTarget(std::mt19937_64 &rng, unsigned maxTarget, unsigned long long total) {
        if (total == 0) {
            return 0;
        }

        std::uniform_int_distribution<unsigned> coinFlip(0, 1);
        if (coinFlip(rng) == 0) {
            std::uniform_int_distribution<unsigned> reachDist(0, static_cast<unsigned>(total));
            return reachDist(rng);
        }

        std::uniform_int_distribution<unsigned> broadDist(0, maxTarget);
        return broadDist(rng);
    }

    bool validateConstraints(const TableData &data, const std::vector<unsigned> &counts, unsigned target) {
        if (data.denom.size() != counts.size()) {
            return false;
        }

        std::vector<long long> remainders(data.denom.size());
        long long rem = target;
        for (std::size_t i = 0; i < data.denom.size(); ++i) {
            rem -= static_cast<long long>(counts[i]) * static_cast<long long>(data.denom[i]);
            if (rem < 0) {
                return false;
            }
            remainders[i] = rem;
        }

        if (rem != 0) {
            return false;
        }

        for (std::size_t i = 0; i < data.denom.size(); ++i) {
            std::size_t finishIndex = i + data.depth[i];
            if (finishIndex >= data.denom.size()) {
                return false;
            }
            if (remainders[finishIndex] >= static_cast<long long>(data.denom[i])) {
                return false;
            }
        }

        return true;
    }

    struct ExhaustiveOutcome {
        bool feasible = false;
        std::optional<unsigned> minCoins;
    };

    ExhaustiveOutcome exhaustiveSearch(const TableData &data, unsigned target) {
        const std::size_t n = data.denom.size();
        std::vector<unsigned> counts(n, 0);
        std::vector<unsigned long long> suffixMax(n + 1, 0);
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            suffixMax[i] = suffixMax[i + 1] +
                           static_cast<unsigned long long>(data.denom[i]) * data.stock[i];
        }

        unsigned bestCoins = std::numeric_limits<unsigned>::max();
        ExhaustiveOutcome outcome;

        std::function<void(std::size_t, unsigned, unsigned)> dfs =
                [&](std::size_t idx, unsigned remaining, unsigned coins) {
                    if (remaining > suffixMax[idx]) {
                        return; // Cannot reach target with leftover stock.
                    }
                    if (coins >= bestCoins) {
                        return; // Prune when already worse than best found.
                    }

                    if (idx == n) {
                        if (remaining != 0) {
                            return;
                        }
                        if (!validateConstraints(data, counts, target)) {
                            return;
                        }
                        outcome.feasible = true;
                        bestCoins = std::min(bestCoins, coins);
                        return;
                    }

                    const unsigned denom = data.denom[idx];
                    const unsigned maxUse = std::min<unsigned>(data.stock[idx], remaining / denom);

                    if (remaining == 0) { // Only zero fits; any positive would exceed target.
                        counts[idx] = 0;
                        dfs(idx + 1, remaining, coins);
                        counts[idx] = 0;
                        return;
                    }

                    for (unsigned use = 0; use <= maxUse; ++use) {
                        counts[idx] = use;
                        dfs(idx + 1, remaining - use * denom, coins + use);
                    }
                    counts[idx] = 0;
                };

        dfs(0, target, 0);

        if (outcome.feasible) {
            outcome.minCoins = bestCoins;
        }
        return outcome;
    }

    struct GreedyEvaluation {
        bool reachable = false;
        unsigned coinCount = 0;
        unsigned sum = 0;
        bool constraintSatisfied = false;
    };

    GreedyEvaluation evaluateGreedy(const TableData &data, unsigned target, const Result &result) {
        GreedyEvaluation eval;
        if (!result.has_value()) {
            return eval;
        }

        eval.reachable = true;
        std::vector<unsigned> counts(data.denom.size(), 0);
        const Used &used = result.value();

        for (std::size_t i = 0; i < data.denom.size(); ++i) {
            auto it = used.find(data.denom[i]);
            if (it != used.end()) {
                counts[i] = it->second;
                eval.coinCount += it->second;
                eval.sum += data.denom[i] * it->second;
            }
        }

        eval.constraintSatisfied = validateConstraints(data, counts, target);
        return eval;
    }

} // namespace

int main(int argc, char **argv) {
    const unsigned defaultTrials = 2000;
    const unsigned defaultMaxStock = 4;
    const unsigned defaultMaxChain = 5;
    const unsigned defaultMaxTarget = 50000;

    const unsigned trials = (argc > 1) ? toUnsigned(argv[1], defaultTrials) : defaultTrials;
    const unsigned maxStock = (argc > 2) ? toUnsigned(argv[2], defaultMaxStock) : defaultMaxStock;
    const unsigned maxChain = (argc > 3) ? toUnsigned(argv[3], defaultMaxChain) : defaultMaxChain;
    const unsigned maxTarget = (argc > 4) ? toUnsigned(argv[4], defaultMaxTarget) : defaultMaxTarget;

    GreedyCoinChangeProblemImprovedVersion solver;
    Summary summary;

    std::mt19937_64 rng(std::random_device{}());

    for (unsigned i = 0; i < trials; ++i) {
        DenominationTable table = randomTable(rng, maxStock, maxChain);
        TableData data = toTableData(table);
        unsigned long long total = totalValue(data);
        unsigned target = pickTarget(rng, maxTarget, total);

        Result greedyResult = solver.Excute(table, static_cast<int>(target));
        ExhaustiveOutcome exhaustive = exhaustiveSearch(data, target);
        GreedyEvaluation eval = evaluateGreedy(data, target, greedyResult);

        ++summary.trials;

        if (eval.reachable != exhaustive.feasible) {
            ++summary.feasibilityMismatch;
            continue;
        }

        if (!eval.reachable) {
            continue; // Both unreachable.
        }

        if (eval.sum != target) {
            ++summary.sumMismatch;
            continue;
        }

        if (!eval.constraintSatisfied) {
            ++summary.constraintMismatch;
            continue;
        }

        if (exhaustive.minCoins.has_value() && eval.coinCount != *exhaustive.minCoins) {
            ++summary.optimalityMismatch;
        }
    }

    const auto pct = [](unsigned part, unsigned total) -> double {
        return total == 0 ? 0.0 : (static_cast<double>(part) * 100.0) / static_cast<double>(total);
    };

    std::cout << "Trials: " << summary.trials << '\n';
    std::cout << "Feasibility mismatches: " << summary.feasibilityMismatch
              << " (" << pct(summary.feasibilityMismatch, summary.trials) << "%)\n";
    std::cout << "Sum mismatches: " << summary.sumMismatch
              << " (" << pct(summary.sumMismatch, summary.trials) << "%)\n";
    std::cout << "Constraint mismatches: " << summary.constraintMismatch
              << " (" << pct(summary.constraintMismatch, summary.trials) << "%)\n";
    std::cout << "Optimality mismatches: " << summary.optimalityMismatch
              << " (" << pct(summary.optimalityMismatch, summary.trials) << "%)\n";

    if (summary.feasibilityMismatch == 0 &&
        summary.sumMismatch == 0 &&
        summary.constraintMismatch == 0 &&
        summary.optimalityMismatch == 0) {
        std::cout << "Improved greedy solver matched exhaustive constrained search in all trials.\n";
        return 0;
    }

    std::cout << "Discrepancies detected; investigate greedy logic or constraint handling.\n";
    return 1;
}
