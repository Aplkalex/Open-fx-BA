/**
 * Open fx-BA: TI BA II Plus Clone
 * cashflow.h - Cash Flow analysis (NPV, IRR, NFV)
 */

#ifndef CASHFLOW_H
#define CASHFLOW_H

#include "types.h"

/* ============================================================
 * Cash Flow List Management
 * ============================================================ */

/**
 * Initialize cash flow list
 */
void cf_init(CashFlowList *cf);

/**
 * Set initial cash flow (CF0)
 */
void cf_set_cf0(CashFlowList *cf, double amount);

/**
 * Add a cash flow group
 * @param amount Cash flow amount
 * @param frequency Number of times this flow occurs (F01, F02, etc.)
 * @return Index of the added flow, or -1 if list is full
 */
int cf_add(CashFlowList *cf, double amount, int frequency);

/**
 * Update an existing cash flow
 * @param index Index of the cash flow (0-based, not including CF0)
 */
void cf_update(CashFlowList *cf, int index, double amount, int frequency);

/**
 * Delete a cash flow
 */
void cf_delete(CashFlowList *cf, int index);

/**
 * Get total number of individual cash flows (considering frequencies)
 */
int cf_total_periods(CashFlowList *cf);

/* ============================================================
 * NPV / IRR / NFV Calculations
 * ============================================================ */

/**
 * Calculate Net Present Value
 * @param cf Cash flow list
 * @param rate Discount rate per period (not %)
 * @return NPV
 */
double cf_npv(CashFlowList *cf, double rate);

/**
 * Calculate Internal Rate of Return using Newton-Raphson
 * @param cf Cash flow list
 * @param errorCode Output: set to non-zero if no solution
 * @return IRR as decimal (multiply by 100 for %)
 */
double cf_irr(CashFlowList *cf, int *errorCode);

/**
 * Calculate Net Future Value (Pro only)
 * NFV = NPV * (1 + rate)^n
 */
double cf_nfv(CashFlowList *cf, double rate);

/**
 * Calculate Payback Period (Pro only)
 * Number of periods to recover initial investment
 */
double cf_payback(CashFlowList *cf);

/**
 * Calculate Discounted Payback Period (Pro only)
 */
double cf_discounted_payback(CashFlowList *cf, double rate);

/**
 * Calculate Modified IRR (Pro only)
 * @param financeRate Rate for negative cash flows
 * @param reinvestRate Rate for positive cash flows
 */
double cf_mirr(CashFlowList *cf, double financeRate, double reinvestRate,
               int *errorCode);

#endif /* CASHFLOW_H */
