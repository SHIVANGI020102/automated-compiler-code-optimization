#include "orchestrator.h"
#include "analyzer.h"
#include "optimizer.h"
#include "verifier.h"
#include "cost.h"
#include "logger.h"
#include "csv.h"
#include <stdio.h>

static int g_stats[5][4]; /* proposals, verified, accepted, rejected */

static int priority(OptimizationType t) {
    switch (t) {
        case OPT_CONSTANT_FOLDING: return 1;
        case OPT_ALGEBRAIC: return 2;
        case OPT_CSE: return 3;
        case OPT_COPY_PROPAGATION: return 4;
        case OPT_DEAD_CODE: return 5;
        default: return 99;
    }
}

static int choose(const OptimizationOpportunity *a, int n) {
    int best = -1;
    for (int i = 0; i < n; ++i) {
        if (best < 0 || priority(a[i].type) < priority(a[best].type)) best = i;
    }
    return best;
}

void reset_global_optimization_stats(void) { for (int i=0;i<5;i++) for (int j=0;j<4;j++) g_stats[i][j]=0; }

void get_global_optimization_stats(int index, int *proposals, int *verified, int *accepted, int *rejected) {
    if (index < 0 || index >= 5) return;
    if (proposals) *proposals = g_stats[index][0];
    if (verified) *verified = g_stats[index][1];
    if (accepted) *accepted = g_stats[index][2];
    if (rejected) *rejected = g_stats[index][3];
}

static int stat_index(OptimizationType t) {
    return (int)t;
}

OptimizationSummary optimize_program(TACProgram *program, int verbose) {
    OptimizationSummary s = {0};
    if (!program) return s;
    TACProgram original;
    copy_program(program, &original);
    s.original_cost = calculate_cost(program).cost;

    /* Every iteration is a fresh Analyze -> Propose -> Verify -> Cost -> Decide cycle. */
    for (int guard = 0; guard < 100; ++guard) {
        OptimizationOpportunity ops[1024];
        int n = 0;
        analyze_program(program, ops, &n);
        if (n == 0) break;

        int chosen = choose(ops, n);
        if (chosen < 0) break;
        OptimizationOpportunity *o = &ops[chosen];
        s.iterations++;

        if (verbose) log_iteration_header(s.iterations, program);
        printf("\nANALYSIS\nDetected %d opportunities. Selected: %s at instruction %d.\n",
               n, optimization_name(o->type), o->instruction_index + 1);
        printf("\nOPTIMIZATION PROPOSAL\n");
        print_instruction(&program->code[o->instruction_index], o->instruction_index);
        printf("Type: %s\n", optimization_name(o->type));

        TACProgram candidate;
        int proposal_ok = propose_optimization(program, o, &candidate);
        int before = calculate_cost(program).cost;
        int after = before;
        VerificationResult vr = {0};
        s.proposals++;
        g_stats[stat_index(o->type)][0]++;

        if (!proposal_ok) {
            s.rejected++;
            g_stats[stat_index(o->type)][3]++;
            printf("Proposal could not be generated. REJECTED.\n");
            log_decision(program->id, s.iterations, o->type, o->instruction_index,
                         0, before, after, "REJECTED", "Proposal generation failed");
            csv_iteration(program->id, s.iterations, before, after, optimization_name(o->type), 0);
            /* No valid candidate exists; continue only if another opportunity can be selected. */
            if (n == 1) break;
            continue;
        }

        after = calculate_cost(&candidate).cost;
        vr = verify_programs(program, &candidate, 20, verbose);
        if (vr.passed) { s.verified++; g_stats[stat_index(o->type)][1]++; }

        printf("\nVERIFICATION\nResult: %s (%d/20 tests passed)\n",
               vr.passed ? "VERIFIED" : "INVALID", vr.passed ? 20 : vr.first_failure - 1);
        printf("\nCOST EVALUATION\nBefore Cost: %d\nAfter Cost: %d\n", before, after);

        int accept = vr.passed && after < before;
        if (accept) {
            s.accepted++;
            g_stats[stat_index(o->type)][2]++;
            *program = candidate;
            printf("Cost Reduction: %.2f%%\n", before ? 100.0 * (before - after) / before : 0.0);
            printf("\nFINAL DECISION\nACCEPTED\nReason: Optimization is correct and reduces cost.\n");
            log_decision(program->id, s.iterations, o->type, o->instruction_index,
                         1, before, after, "ACCEPTED", "Correct and cheaper");
        } else {
            s.rejected++;
            g_stats[stat_index(o->type)][3]++;
            const char *reason = vr.passed ? "New cost is not lower" : "Verification failed";
            printf("\nFINAL DECISION\nREJECTED\nReason: %s.\n", reason);
            log_decision(program->id, s.iterations, o->type, o->instruction_index,
                         vr.passed, before, after, "REJECTED", reason);
        }
        csv_iteration(program->id, s.iterations, before, after, optimization_name(o->type), accept);

        /* If the highest-priority proposal was rejected, trying the same candidate again
           would loop forever. The safe rule-based implementation stops this pass. */
        if (!accept) break;
    }

    s.final_cost = calculate_cost(program).cost;
    long long zero_inputs[7] = {0,0,0,0,0,0,0};
    long long original_output = 0, final_output = 0;
    int a = execute_program(&original, zero_inputs, &original_output);
    int b = execute_program(program, zero_inputs, &final_output);
    s.output_match = a && b && (original_output == final_output);
    return s;
}
