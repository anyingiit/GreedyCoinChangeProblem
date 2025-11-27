#include "GreedyCoinChangeProblem.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    using Stock = GreedyCoinChangeProblem::Stock;
    using Used = GreedyCoinChangeProblem::Used;
    using Result = GreedyCoinChangeProblem::Result;

    Stock makeStock(unsigned defaultCount) {
        Stock stock;
        for (auto denom: GreedyCoinChangeProblem::kStandardDenominations) {
            stock[denom] = defaultCount;
        }
        return stock;
    }

    unsigned totalValue(const Used &used) {
        unsigned total = 0;
        for (auto [denom, count]: used) {
            total += denom * count;
        }
        return total;
    }

    void assertTrue(bool condition, const std::string &message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    void expectSuccessWithBreakdown() {
        GreedyCoinChangeProblem solver;
        Stock stock = {
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

        const int target = 186;
        Result result = solver.Excute(stock, target);
        assertTrue(result.has_value(), "Expected success for feasible stock");

        Used expected = {
                {10000, 0},
                {5000,  0},
                {1000,  0},
                {500,   0},
                {100,   1},
                {50,    1},
                {10,    3},
                {5,     1},
                {1,     1}
        };

        assertTrue(result.value() == expected, "Greedy breakdown does not match expectation");
        assertTrue(totalValue(result.value()) == static_cast<unsigned>(target), "Total does not match target");
    }

    void expectFailureWhenSizeWrong() {
        GreedyCoinChangeProblem solver;
        Stock stock = makeStock(1);
        stock.erase(1);  // Remove denomination to trigger size check.

        Result result = solver.Excute(stock, 100);
        assertTrue(!result.has_value(), "Expected failure due to stock size");
        assertTrue(result.error() == "stock size illgle", "Unexpected error message for size check");
    }

    void expectFailureWhenKeyMissing() {
        GreedyCoinChangeProblem solver;
        Stock stock = {
                {10000, 1},
                {5000,  1},
                {1000,  1},
                {500,   1},
                {100,   2},
                {50,    2},
                {10,    5},
                {5,     3},
                {2,     10} // Wrong key instead of 1.
        };

        Result result = solver.Excute(stock, 50);
        assertTrue(!result.has_value(), "Expected failure because required key is missing");
        assertTrue(result.error() == "stock not has required key-value", "Unexpected error message for missing key");
    }

    void expectFailureWhenStockTooSmall() {
        GreedyCoinChangeProblem solver;
        Stock stock = makeStock(0);
        stock[100] = 1; // Not enough to reach target.

        Result result = solver.Excute(stock, 200);
        assertTrue(!result.has_value(), "Expected failure when stock is insufficient");
        assertTrue(result.error() == "not has enothod stock to handle T",
                   "Unexpected error message for insufficient stock");
    }

    void expectZeroTargetHandled() {
        GreedyCoinChangeProblem solver;
        Stock stock = makeStock(3);
        Result result = solver.Excute(stock, 0);
        assertTrue(result.has_value(), "Zero target should succeed with zero usage");
        assertTrue(totalValue(result.value()) == 0, "Total for zero target should be zero");
    }

} // namespace

int main() {
    struct TestCase {
        std::string name;
        std::function<void()> fn;
    };

    std::vector<TestCase> tests = {
            {"expectSuccessWithBreakdown",     expectSuccessWithBreakdown},
            {"expectFailureWhenSizeWrong",     expectFailureWhenSizeWrong},
            {"expectFailureWhenKeyMissing",    expectFailureWhenKeyMissing},
            {"expectFailureWhenStockTooSmall", expectFailureWhenStockTooSmall},
            {"expectZeroTargetHandled",        expectZeroTargetHandled},
    };

    unsigned failures = 0;
    for (const auto &test: tests) {
        try {
            test.fn();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception &ex) {
            ++failures;
            std::cout << "[FAIL] " << test.name << ": " << ex.what() << '\n';
        }
    }

    if (failures == 0) {
        std::cout << "All " << tests.size() << " tests passed.\n";
        return 0;
    }

    std::cout << failures << " test(s) failed out of " << tests.size() << ".\n";
    return 1;
}
