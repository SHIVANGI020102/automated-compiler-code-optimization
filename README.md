# Automated LLM/Agent-Guided Compiler Code Optimization — C

## 1. Aim
This project demonstrates an autonomous compiler-optimization agent using Three Address Code (TAC). The first version uses a rule-based Optimization Specialist, so it has no external LLM/API dependency.

## 2. Architecture
**Orchestrator → Code Analysis Specialist → Optimization Specialist → Verification Module → Cost Evaluator → Accept/Reject → repeat**.

- **Code Analysis Specialist:** finds constant folding, algebraic simplification, CSE, copy propagation and dead-code opportunities.
- **Optimization Specialist:** proposes exactly one candidate transformation.
- **Verification Module:** runs 20 deterministic-random differential tests through a TAC interpreter and compares outputs.
- **Cost Evaluator:** `instruction_count + arithmetic_operations + temporary_variables`.
- **Orchestrator:** accepts only `VERIFIED && new_cost < old_cost`.

## 3. TAC
Examples: `t1 = a + b`, `t2 = t1 * c`, `t3 = a`. Variables include `a,b,c,d,x,y,z` and temporaries `t1...`.

## 4. Optimizations
1. Constant folding: `5 + 3 → 8`.
2. Algebraic simplification: `x+0 → x`, `x*1 → x`, `x*0 → 0`, `x/1 → x`.
3. CSE: a repeated straight-line expression reuses an earlier result.
4. Copy propagation: `t1=a; t2=t1+b` becomes `t2=a+b`, with the copy removed when safe.
5. Dead-code elimination: assignments whose values are never used and are not the program output are removed.

## 5. Verification
For every candidate, the original and candidate TAC programs are executed on 20 generated input vectors. Any mismatch rejects the candidate. This is differential testing; it is strong empirical evidence, not a formal mathematical proof for all possible inputs.

## 6. Cost
`COST = instructions + arithmetic_operations + temporary_variables`.
A candidate is accepted only when its cost is strictly smaller.

## 7. Dataset
`dataset/programs.txt` is generated automatically with 200–500 valid programs. Categories are Arithmetic, Nested, CSE, DeadCode, Loop, Conditional and Mixed. Loop/Conditional categories are represented as deterministic TAC workloads in this college-level first version; the optimizer remains conservative around control flow.

## 8. Metrics
The executable calculates verification pass rate, acceptance rate, average cost reduction and average iterations. It prints LLM validity as N/A because this build is rule-based.

## 9. CSV output
- `results.csv`: one row per program.
- `decision_log.csv`: every optimization attempt.
- `optimization_stats.csv`: summary table placeholder rows; use `decision_log.csv` for exact per-type aggregation.
- `iteration_cost.csv`: cost before/after each attempted optimization.
- `category_stats.csv`: category averages.

## 10. Compilation
```bash
make
# or:
gcc -Wall -Wextra -std=c11 -O2 -o optimizer main.c tac.c analyzer.c optimizer.c verifier.c cost.c orchestrator.c dataset.c logger.c csv.c
```

## 11. Execution
```bash
./optimizer
./optimizer --demo
./optimizer --count 500
./optimizer --demo --verbose
```

## 12. Demonstration
The demo contains constant folding, algebraic simplification, CSE, copy/dead-code opportunities and visibly repeats the agent loop.

## 13. Plotting the CSVs
The C program does not depend on Python. After execution, Excel, MATLAB, R or Python can read the CSVs.

- **Sankey:** use `decision_log.csv`; aggregate proposal → verified → accepted.
- **Bar chart:** group `decision_log.csv` by optimization and decision.
- **Line chart:** plot `iteration` vs `cost_after` from `iteration_cost.csv` for selected program IDs.
- **Pie chart:** count accepted rows by optimization type in `decision_log.csv`.
- **Scatter:** `iteration`/total steps against cost reduction from `results.csv`.
- **Heatmap:** category × optimization type using grouped accepted rows from the decision log.

## 14. Limitations
Differential testing is not formal verification. The rule-based analyzer is deliberately simple. The current dataset's Loop and Conditional labels are representative TAC workloads rather than a full CFG optimizer. CSE is restricted to straight-line regions to avoid unsound cross-control-flow transformations.

## 15. Future LLM integration
Replace only the Optimization Specialist with an adapter that sends an opportunity plus TAC to an LLM and parses a candidate TAC. The candidate must still pass the same independent verifier and cost gate. The LLM is never trusted merely because it claims correctness.

## 16. Viva points
- TAC breaks complex expressions into simple instructions.
- An agent architecture separates analysis, proposal, verification and decision making.
- Differential testing compares original and transformed programs on identical inputs.
- Cost is intentionally simple and measurable for a college project.
- The acceptance invariant is **correct + cheaper = accept**.
