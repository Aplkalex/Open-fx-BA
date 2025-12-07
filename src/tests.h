/**
 * Open fx-BA: TI BA II Plus Clone
 * tests.h - CFA-style test cases for validation
 *
 * Contains 10 test cases based on CFA exam-style questions
 * to verify calculator accuracy.
 */

#ifndef TESTS_H
#define TESTS_H

#include "types.h"

/* ============================================================
 * Test Result Structure
 * ============================================================ */
typedef struct {
  const char *name;  /* Test name */
  const char *level; /* CFA Level (I, II, III, BOSS) */
  int passed;        /* 1 = passed, 0 = failed */
  double expected;   /* Expected result */
  double actual;     /* Actual result */
  double tolerance;  /* Acceptable error margin */
} TestResult;

typedef struct {
  int total;              /* Total tests run */
  int passed;             /* Tests passed */
  int failed;             /* Tests failed */
  TestResult results[50]; /* Individual results - increased for new tests */
} TestSuite;

/* ============================================================
 * Test Runner Functions
 * ============================================================ */

/**
 * Run all CFA test cases
 * @param suite Output: test results
 */
void tests_run_all(TestSuite *suite);

/**
 * Run only TVM tests (Q1-Q3, Q6, Q7)
 */
void tests_run_tvm(TestSuite *suite);

/**
 * Run only Cash Flow tests (Q4, Q5, Q9)
 */
void tests_run_cashflow(TestSuite *suite);

/**
 * Run only advanced tests (Q8, Q10)
 */
void tests_run_advanced(TestSuite *suite);

/**
 * Print test results to console
 */
void tests_print_results(TestSuite *suite);

/**
 * Check if a value matches expected within tolerance
 */
int tests_check_value(double expected, double actual, double tolerance);

/* ============================================================
 * Individual Test Cases
 * ============================================================ */

/* Level I Tests */
TestResult test_q1_mortgage_payment(void);
TestResult test_q2_retirement_savings(void);
TestResult test_q3_present_value(void);

/* Level II Tests */
TestResult test_q4_npv_project(void);
TestResult test_q5_irr_project(void);
TestResult test_q6_bond_pricing(void);

/* Level III Tests */
TestResult test_q7_annuity_due(void);
TestResult test_q8_loan_amortization(void);
TestResult test_q9_uneven_cashflows(void);

/* Boss Level Test */
TestResult test_q10_retirement_planning(void);

/* ============================================================
 * Additional Test Cases (Set 2)
 * ============================================================ */

/* Level I - Set 2 */
TestResult test_s2_q1_annuity_due_fv(void); /* $2000/yr BGN mode, 8%, 15yrs */
TestResult test_s2_q2_npv_project(void);    /* NPV with 3 cash flows */
TestResult test_s2_q3_bond_ytm(void);       /* Bond YTM calculation */

/* Level II - Set 2 */
TestResult test_s2_q4_bond_dirty_price(void); /* Dirty price calculation */
TestResult test_s2_q5_multistage_ddm(void);   /* Dividend Discount Model */
TestResult test_s2_q6_sample_stddev(void);    /* Sample standard deviation */

/* Level III - Set 2 */
TestResult test_s2_q7_duration_convexity(void); /* Duration & Convexity */
TestResult test_s2_q8_portfolio_stddev(void);   /* Two-asset portfolio risk */
TestResult test_s2_q9_forward_valuation(void);  /* Forward contract value */

/* Hardest - Set 2 */
TestResult test_s2_q10_horizon_yield(void); /* Total return calculation */

#endif /* TESTS_H */
