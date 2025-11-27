//
// Created by User laptop on 2025/11/26.
//

#ifndef GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H
#define GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H

#include <array>
#include <map>
#include <expected>
#include <string>

class GreedyCoinChangeProblem {
public:
    static constexpr std::array<unsigned, 9> kStandardDenominations = {
            10000, 5000, 1000, 500, 100, 50, 10, 5, 1
    };

    using Stock = std::map<unsigned, unsigned>;   // 库存：面额 -> 张数
    using Used  = std::map<unsigned, unsigned>;   // 实际使用：面额 -> 张数
    using Result = std::expected<Used, std::string>;

    GreedyCoinChangeProblem();

    Result Excute(const Stock& stock, int T);
};


#endif //GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H
