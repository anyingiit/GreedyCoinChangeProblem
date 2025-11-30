# Greedy Coin Change Problem / 贪心找零问题

## Overview / 概览

- Implements a greedy change-maker over a fixed denomination set, returning either a breakdown or an error if stock is
  insufficient. / 基于固定面额集合的贪心找零实现，返回找零明细或在库存不足时给出错误。
- Written in C++23 with a minimal interface `GreedyCoinChangeProblem::Excute(stock, T)` producing `std::expected`. / 使用
  C++23 编写，核心接口 `GreedyCoinChangeProblem::Excute(stock, T)`，返回 `std::expected`。
- Includes a sample app (`GreedyCoinChangeProblem`) plus two tooling executables: unit tests and statistical
  validation. / 附带示例程序（`GreedyCoinChangeProblem`）以及单元测试与统计验证两个工具可执行文件。

## Build / 构建

- Prerequisites: CMake ≥ 3.31, C++23-capable compiler (e.g., GCC 13/Clang 16/VS 2022). / 依赖：CMake ≥ 3.31，支持 C++23
  的编译器（如 GCC 13/Clang 16/VS 2022）。
- Build (Release example): / 构建（Release 示例）：
    - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`
    - `cmake --build build`

## Run sample app / 运行示例程序

- After build: `./build/GreedyCoinChangeProblem` prints a breakdown for a demo target (186) and stock. / 构建后运行
  `./build/GreedyCoinChangeProblem`，输出示例目标值（186）与库存的找零分解。

## Tests / 测试

- Unit tests:
    - `./build/GreedyCoinChangeTests`: deterministic coverage of success/error cases for the base solver. / 基础求解器的确定性用例。
    - `./build/GreedyCoinChangeImprovedVersionTests`: depth-constrained denomination chain coverage (divisibility, depth
      bounds, negative targets, insufficient stock). / 含最大向下委托深度的整除链测试，覆盖整除性、深度越界、负目标与库存不足。
- Statistical validation:
    - `./build/GreedyCoinChangeValidation [trials=5000] [maxCount=8] [maxTarget=20000]` compares greedy results against
      bounded-optimal DP on random stocks/targets. / 在随机库存与目标上将贪心结果与有界最优 DP 对比。
    - `./build/GreedyCoinChangeImprovedValidation [trials=2000] [maxStock=4] [maxChainLen=5] [maxTarget=50000]`
      exhaustively checks depth-constrained greedy against all feasible allocations of random divisible chains, and
      asserts coin-count optimality. / 对随机整除链穷举验证深度约束贪心的可行性与最优币数。
- Exit code 0 means all checks passed; non-zero indicates mismatches or detected inconsistencies. / 返回码 0
  表示全部通过；非零表示存在不一致或缺陷。

## Files / 文件

- `GreedyCoinChangeProblem.h/.cpp`: Core solver with exposed standard denominations. / 核心求解器，公开标准面额数组。
- `GreedyCoinChangeProblemImprovedVersion.h/.cpp`: Solver variant with max-fallback-depth constraints per denomination. /
  含最大向下委托深度约束的求解器版本。
- `main.cpp`: Example invocation printing a breakdown. / 示例调用并打印找零分解。
- `GreedyCoinChangeProblemTests.cpp`: Deterministic unit tests for the base solver. / 基础求解器单元测试。
- `GreedyCoinChangeProblemImprovedVersionTests.cpp`: Deterministic unit tests for the depth-constrained solver. /
  深度约束求解器单元测试。
- `GreedyCoinChangeValidation.cpp`: Monte Carlo validator against optimal DP. / 基于最优 DP 的蒙特卡洛验证器。
- `GreedyCoinChangeProblemImprovedValidation.cpp`: Exhaustive validator for the depth-constrained solver. / 深度约束求解器的穷举验证器。
- `CMakeLists.txt`: Targets for sample app, tests, and validation. / 定义示例、测试与验证可执行文件的构建规则。

## Usage notes / 使用说明

- Stock must contain exactly the fixed 9 denominations; mismatches return descriptive errors. / 库存必须包含固定的 9
  种面额，缺失或数量不符会返回可读错误。
- On success, the returned map lists coins used per denomination; sum equals target or validation will fail. /
  成功时返回按面额的使用数量；总额需等于目标，否则验证会失败。

## Extending / 扩展

- Add or change denominations by updating `kStandardDenominations`; adjust validation if denomination set changes. /
  通过修改 `kStandardDenominations` 变更面额，变更后需同步验证逻辑。
- For larger targets/stocks, the DP in validation remains bounded but may grow in memory; tune maxTarget accordingly. /
  对于更大目标或库存，验证用的有界 DP 可能增大内存占用，可调整 maxTarget 以平衡性能。 
