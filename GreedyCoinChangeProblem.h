//
// Created by User laptop on 2025/11/26.
//

#ifndef GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H
#define GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H

#include <map>
#include <expected>
#include <string>

class GreedyCoinChangeProblem {
public:
    using Stock = std::map<unsigned, unsigned>;   // 库存：面额 -> 张数
    using Used  = std::map<unsigned, unsigned>;   // 实际使用：面额 -> 张数
    using Result = std::expected<Used, std::string>;

    GreedyCoinChangeProblem();

    Result Excute(const Stock& stock, int T);
};


#endif //GREEDYCOINCHANGEPROBLEM_GREEDYCOINCHANGEPROBLEM_H
