#include "GreedyCoinChangeProblemImprovedVersion.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    using DenominationTable = GreedyCoinChangeProblemImprovedVersion::DenominationTable;
    using Used = GreedyCoinChangeProblemImprovedVersion::Used;
    using Result = GreedyCoinChangeProblemImprovedVersion::Result;

    void assertTrue(bool condition, const std::string &message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    unsigned totalValue(const Used &used) {
        unsigned total = 0;
        for (auto [denom, count]: used) {
            total += denom * count;
        }
        return total;
    }

    void expectSuccessWithinDepth() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {5, 0}},
                {500,  {3, 1}},
                {1000, {1, 1}},
        };

        const int target = 2600;
        Result result = solver.Excute(table, target);
        assertTrue(result.has_value(), "Expected success when fallback depth is sufficient");

        Used expected = {
                {100,  1},
                {500,  3},
                {1000, 1},
        };

        assertTrue(result.value() == expected, "Unexpected breakdown for feasible depth-bounded plan");
        assertTrue(totalValue(result.value()) == static_cast<unsigned>(target), "Sum should match target");
    }

    void expectFailureWhenDepthExceeded() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {11, 0}},
                {500,  {1,  1}},
                {1000, {1,  1}},
        };

        Result result = solver.Excute(table, 2600);
        assertTrue(!result.has_value(), "Expected failure because remainder exceeds allowed fallback depth");
        assertTrue(result.error() ==
                   "error: denomination 1000 must be fully resolved by level 500, but remainder 1100 >= 1000",
                   "Unexpected error message for fallback depth violation");
    }

    void expectFailureForNonDivisibleChain() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {10, 0}},
                {600,  {2,  1}}, // 1000 % 600 != 0, should fail divisibility chain check.
                {1000, {1,  1}},
        };

        Result result = solver.Excute(table, 1200);
        assertTrue(!result.has_value(), "Expected failure due to non-divisible denomination chain");
        assertTrue(result.error() == "error key illegal", "Unexpected error message for divisibility violation");
    }

    void expectFailureForInvalidFallbackDepth() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {5, 0}},
                {500,  {2, 1}},
                {1000, {1, 3}}, // Depth points beyond the end of the chain.
        };

        Result result = solver.Excute(table, 1000);
        assertTrue(!result.has_value(), "Expected failure due to invalid fallback depth");
        assertTrue(result.error() == "error maxFallbackDepth illegal", "Unexpected error message for depth bounds");
    }

    void expectFailureForNegativeTarget() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {5, 0}},
                {500,  {5, 1}},
                {1000, {5, 1}},
        };

        Result result = solver.Excute(table, -1);
        assertTrue(!result.has_value(), "Negative target should be rejected");
        assertTrue(result.error() == "error T illegal, must >= 0", "Unexpected error for negative target");
    }

    void expectFailureWhenStockInsufficient() {
        GreedyCoinChangeProblemImprovedVersion solver;
        DenominationTable table = {
                {100,  {1, 0}},
                {500,  {0, 1}},
                {1000, {0, 1}},
        };

        Result result = solver.Excute(table, 500);
        assertTrue(!result.has_value(), "Expected failure when stock cannot cover target");
        assertTrue(result.error() == "error: not enough stock to handle T exactly",
                   "Unexpected error message for insufficient stock");
    }

} // namespace

int main() {
    struct TestCase {
        std::string name;
        std::function<void()> fn;
    };

    std::vector<TestCase> tests = {
            {"expectSuccessWithinDepth",             expectSuccessWithinDepth},
            {"expectFailureWhenDepthExceeded",       expectFailureWhenDepthExceeded},
            {"expectFailureForNonDivisibleChain",    expectFailureForNonDivisibleChain},
            {"expectFailureForInvalidFallbackDepth", expectFailureForInvalidFallbackDepth},
            {"expectFailureForNegativeTarget",       expectFailureForNegativeTarget},
            {"expectFailureWhenStockInsufficient",   expectFailureWhenStockInsufficient},
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
        std::cout << "All " << tests.size() << " improved-version tests passed.\n";
        return 0;
    }

    std::cout << failures << " test(s) failed out of " << tests.size() << ".\n";
    return 1;
}
