//
// Created by User laptop on 2025/11/28.
//

#ifndef GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEMIMPROVEDVERSION_H
#define GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEMIMPROVEDVERSION_H

#include <array>
#include <map>
#include <expected>
#include <string>

class GreedyCoinChangeProblemImprovedVersion {
public:

    struct DenominationPolicy {
        unsigned stock;            // 库存数量
        unsigned maxFallbackDepth; // 最大向下替代深度
    };

    using DenominationTable = std::map<unsigned, DenominationPolicy>;   // 库存：面额 -> 张数. 其中key必须在生序排序后满足整除链
    using Used = std::map<unsigned, unsigned>;   // 实际使用：面额 -> 张数
    using Result = std::expected<Used, std::string>;

    GreedyCoinChangeProblemImprovedVersion();

    Result Excute(const DenominationTable &denominationTable, int T);
};


#endif //GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEMIMPROVEDVERSION_H
