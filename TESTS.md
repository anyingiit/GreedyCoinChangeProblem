# Tests and Validation / 测试与验证

## Unit tests / 单元测试

- Runner: `GreedyCoinChangeTests` (deterministic, no randomness). / 运行器：`GreedyCoinChangeTests`，确定性且无随机因素。
    - `expectSuccessWithBreakdown`: Feasible stock returns the expected greedy breakdown and exact sum to target (
      186). /
      可行库存得到预期分解且总额等于 186。
    - `expectFailureWhenSizeWrong`: Missing denomination triggers size check error. / 缺少面额触发库存大小校验错误。
    - `expectFailureWhenKeyMissing`: Wrong/missing required key reports the key-missing error. / 缺失必需面额触发“缺少键值”错误。
    - `expectFailureWhenStockTooSmall`: Insufficient stock triggers the “not enough stock” error. / 库存不足导致“库存不足”错误。
    - `expectZeroTargetHandled`: Target 0 succeeds with zero usage. / 目标为 0 时应成功且不使用任何币。
- Runner: `GreedyCoinChangeImprovedVersionTests` (depth-constrained chain). / 运行器：
  `GreedyCoinChangeImprovedVersionTests`
  （含最大向下委托深度）。
    - `expectSuccessWithinDepth`: Depth allowance respected and exact breakdown returned. / 深度允许时成功找零并给出明细。
    - `expectFailureWhenDepthExceeded`: Exact sum needs超出允许深度的委托时触发错误消息。 / 依赖更深层面额时返回错误。
    - `expectFailureForNonDivisibleChain`: 非整除链触发 key 校验错误。
    - `expectFailureForInvalidFallbackDepth`: 越界的最大委托深度被拒绝。 / 最大委托深度越界时返回错误。
    - `expectFailureForNegativeTarget`: 负目标值被拒绝。 / 负数目标时返回错误。
    - `expectFailureWhenStockInsufficient`: 库存不足时返回“库存不足”错误。 / 库存无法覆盖目标时失败。
- Output: `[PASS]/[FAIL]` per test; exit code 0 on full pass. / 输出：逐个用例的 `[PASS]/[FAIL]`；全部通过时退出码为 0。

## Statistical validation / 统计验证

- Runner: `GreedyCoinChangeValidation [trials] [maxCount] [maxTarget]`. Defaults: 5000 trials, per-denomination
  maxCount=8, maxTarget=20000. / 运行器：`GreedyCoinChangeValidation [trials] [maxCount] [maxTarget]`，默认 5000
  次试验、每面额库存上限 8、目标上限 20000。
- Method / 方法：
    - Randomly samples stocks (0..maxCount) for the fixed denominations and targets (0..maxTarget). / 在固定面额上为每种币随机生成
      0..maxCount 张数，并随机选取目标 0..maxTarget。
    - Runs greedy solver; in parallel, computes bounded-optimal coin count via binary-split DP. /
      对每次样本运行贪心求解，同时通过二进制拆分的有界 DP 求最优币张数。
    - Compares reachability, sum consistency, and coin-count optimality; accumulates mismatch stats. /
      比较可达性、求和值一致性与最优币数，并累积不一致统计。
- Output / 输出：
    - Reports counts and percentages for reachability mismatches, sum mismatches, and coin-count mismatches. /
      输出可达性、求和值、币数不一致的数量与百分比。
    - Exit code 0 if no mismatches; 1 otherwise. / 无不一致时退出码 0，否则为 1。
- Runner: `GreedyCoinChangeImprovedValidation [trials] [maxStock] [maxChainLen] [maxTarget]`. Defaults: 2000 trials,
  maxStock=4, maxChainLen=5, maxTarget=50000. / 运行器：`GreedyCoinChangeImprovedValidation [trials] [maxStock]
  [maxChainLen] [maxTarget]`，默认 2000 次试验、单面额库存上限 4、链长上限 5、目标上限 50000。
    - Builds random divisible chains with per-denomination stock and maxFallbackDepth; target sampled from reachable and
      broad ranges. / 随机生成整除链及库存、委托深度，并从可达与宽范围混合采样目标。
    - Exhaustively enumerates all stock-bounded allocations to find feasible plans and minimal币数，并校验深度约束。 /
      通过穷举搜索验证是否存在可行方案及最少币数，并检查深度约束。
    - Compares greedy reachability, sum一致性、约束合法性和币数最优性；统计不一致数量。 / 比对贪心与穷举的可达性、求和值、约束与最优币数。
    - Exit code 0 when no mismatches; 1 otherwise. / 无不一致时退出码 0，否则为 1。

## How to run / 运行方式

- Build first: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build`. / 先构建：
  `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build`。
- Run unit tests: `./build/GreedyCoinChangeTests` and `./build/GreedyCoinChangeImprovedVersionTests`. / 运行单元测试：
  `./build/GreedyCoinChangeTests` 以及 `./build/GreedyCoinChangeImprovedVersionTests`。
- Run statistical validation (example 10k trials): `./build/GreedyCoinChangeValidation 10000 8 20000`. Improved version
  example: `./build/GreedyCoinChangeImprovedValidation 3000 4 5 50000`. / 运行统计验证（示例 1 万次）：
  `./build/GreedyCoinChangeValidation 10000 8 20000`；改进版示例
  `./build/GreedyCoinChangeImprovedValidation 3000 4 5 50000`。

## Interpretation / 结果解读

- If unit tests fail, the error message indicates which guard or breakdown is incorrect. / 若单元测试失败，错误信息会指出对应校验或分解问题。
- If validation reports mismatches, investigate greedy correctness or stock assumptions; adjust `maxTarget` for larger
  search space. / 若验证出现不一致，需检查贪心正确性或库存假设，可调高 `maxTarget` 扩大搜索空间。 
