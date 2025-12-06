//
// Created by User laptop on 2025/11/28.
//

#include "GreedyCoinChangeProblemImprovedVersion.h"
#include <array>
#include <algorithm>
#include <vector>
#include <format>


GreedyCoinChangeProblemImprovedVersion::GreedyCoinChangeProblemImprovedVersion() {

}

GreedyCoinChangeProblemImprovedVersion::Result
GreedyCoinChangeProblemImprovedVersion::Excute(const DenominationTable &denominationTable, int T) {
    if (T < 0) {
        return std::unexpected("error T illegal, must >= 0");
    }
    // 将denominationTable纸张进行生序排序，并且排序后的面额从前到后每两个元素必须满足整除， 以达成整除链
    std::vector<unsigned> keys;
    keys.reserve(denominationTable.size());

    // 1. 取出所有 key
    for (const auto &[k, _]: denominationTable) {
        keys.push_back(k);
    }

    // 2. 排序（从大到小，适合找零算法）
    std::ranges::sort(keys, std::greater<>());

    // 3. 检查是否为整除链
    for (int i = 0; i < keys.size() - 1; i++) {
        if (keys[i] % keys[i + 1] != 0) {
            return std::unexpected("error key illegal");
        }
    }

    // 4. 检查maxFallbackDepth是否合法
    for (int i = 0; i < keys.size(); i++) {
        unsigned D = keys[i];
        if (i + denominationTable.at(D).maxFallbackDepth > denominationTable.size() - 1) {
            return std::unexpected("error maxFallbackDepth illegal");
        }
    }

    // 已经完成了面额检查，正式开始处理

    Used used;
    std::vector<unsigned> R(keys.size());

    unsigned remainder = T;

    for (std::size_t index = 0; index < keys.size(); ++index) {
        unsigned D = keys[index];

        unsigned q = remainder / D;
        unsigned g = std::min(q, denominationTable.at(D).stock);
        used.try_emplace(D, g);

        remainder -= g * D;
        R[index] = remainder;
    }

    if (remainder != 0) {
        return std::unexpected("error: not enough stock to handle T exactly");
    }

    for (std::size_t i = 0; i < keys.size(); ++i) {
        unsigned D = keys[i];
        unsigned depth = denominationTable.at(D).maxFallbackDepth;
        std::size_t finishIndex = i + depth; // 前面已经保证不会越界

        if (R[finishIndex] >= D) {
            return std::unexpected(
                    std::format("error: denomination {} must be fully resolved by level {}, "
                                "but remainder {} >= {}",
                                D, keys[finishIndex], R[finishIndex], D)
            );
        }
    }


    return used;
}

