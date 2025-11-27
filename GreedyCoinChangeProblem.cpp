//
// Created by User laptop on 2025/11/26.
//

#include "GreedyCoinChangeProblem.h"
#include <array>
#include <algorithm>


static constexpr std::array<unsigned, 9> kStandardDenoms = {
        10000, 5000, 1000, 500, 100, 50, 10, 5, 1
};

GreedyCoinChangeProblem::GreedyCoinChangeProblem() {

}

GreedyCoinChangeProblem::Result
GreedyCoinChangeProblem::Excute(const GreedyCoinChangeProblem::Stock &stock, int T) {
    if (stock.size() != kStandardDenoms.size()){
        return std::unexpected("stock size illgle");
    }
    for (unsigned D : kStandardDenoms) {
        if (!stock.contains(D)){
            return std::unexpected("stock not has required key-value");
        }
    }

    Used used;
    unsigned R = T;
    for (unsigned D: kStandardDenoms) {
        unsigned q = R / D;
        unsigned g = std::min(q, stock.at(D));
        used.try_emplace(D, g);
        R -= g * D;
    }

    if (R != 0){
        return std::unexpected("not has enothod stock to handle T");
    }

    return used;
}




