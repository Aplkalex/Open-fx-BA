/**
 * Open fx-BA: TI BA II Plus Clone
 * tvm.h - Time Value of Money calculations
 */

#ifndef TVM_H
#define TVM_H

#include "types.h"

/* ============================================================
 * TVM Solver - Main Entry Point
 * ============================================================ */

/**
 * Solve for a specific TVM variable.
 * The other 4 variables must already be set in calc->tvm.
 *
 * @param calc Calculator state
 * @param solveFor Which variable to solve for
 * @return The computed value (also stored in calc->tvm)
 */
double tvm_solve_for(Calculator *calc, TVMVariable solveFor);

/* ============================================================
 * Individual TVM Functions
 * ============================================================ */

/**
 * Calculate Future Value
 * Given: N, I/Y, PV, PMT
 */
double tvm_calc_fv(double n, double rate, double pv, double pmt, TVMMode mode);

/**
 * Calculate Present Value
 * Given: N, I/Y, PMT, FV
 */
double tvm_calc_pv(double n, double rate, double pmt, double fv, TVMMode mode);

/**
 * Calculate Payment
 * Given: N, I/Y, PV, FV
 */
double tvm_calc_pmt(double n, double rate, double pv, double fv, TVMMode mode);

/**
 * Calculate Number of Periods
 * Given: I/Y, PV, PMT, FV
 */
double tvm_calc_n(double rate, double pv, double pmt, double fv, TVMMode mode);

/**
 * Calculate Interest Rate (using Newton-Raphson)
 * Given: N, PV, PMT, FV
 *
 * @param errorCode Output: Set to non-zero if no solution found
 */
double tvm_calc_iy(double n, double pv, double pmt, double fv, TVMMode mode,
                   int *errorCode);

/* ============================================================
 * Amortization
 * ============================================================ */

/**
 * Amortization result for a single period
 */
typedef struct {
  double principal; /* Principal paid this period */
  double interest;  /* Interest paid this period */
  double balance;   /* Remaining balance after this period */
} AmortResult;

/**
 * Calculate amortization for a specific period.
 *
 * @param period Period number (1-based)
 * @param n Total number of periods
 * @param rate Periodic interest rate (not annual %)
 * @param pv Loan amount (positive)
 * @param pmt Payment amount (positive)
 * @return Amortization breakdown for this period
 */
AmortResult tvm_amort_period(int period, double n, double rate, double pv,
                             double pmt);

/**
 * Calculate cumulative amortization for a range of periods.
 *
 * @param startPeriod First period (1-based)
 * @param endPeriod Last period (inclusive)
 * @param ... (same as above)
 * @param totalPrincipal Output: Total principal paid
 * @param totalInterest Output: Total interest paid
 * @param endBalance Output: Balance at end of range
 */
void tvm_amort_range(int startPeriod, int endPeriod, double n, double rate,
                     double pv, double pmt, double *totalPrincipal,
                     double *totalInterest, double *endBalance);

/* ============================================================
 * Helper Functions
 * ============================================================ */

/**
 * Convert annual rate (%) to periodic rate based on P/Y and C/Y.
 */
double tvm_periodic_rate(double annualRate, double py, double cy);

/* Forward declaration for format_number (from ui.c) */
void format_number(double value, char *buffer, int maxLen);

#endif /* TVM_H */
