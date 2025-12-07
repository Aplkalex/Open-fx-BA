/**
 * Open fx-BA: TI BA II Plus Clone
 * profit.c - Breakeven, Cost-Sell-Margin, Percent calculations
 */

#include "profit.h"
#include <math.h>

/* ============================================================
 * Breakeven Analysis
 * ============================================================ */

void breakeven_init(Breakeven *be) {
  be->fixedCost = 0.0;
  be->variableCostPerUnit = 0.0;
  be->pricePerUnit = 0.0;
  be->quantity = 0.0;
  be->profit = 0.0;
  be->revenue = 0.0;
}

double breakeven_calc_quantity(Breakeven *be) {
  /*
   * Breakeven quantity: where Revenue = Total Cost
   * Q * P = FC + Q * VC
   * Q * (P - VC) = FC
   * Q = FC / (P - VC)
   */
  double contribution = be->pricePerUnit - be->variableCostPerUnit;

  if (contribution <= 0.0) {
    /* No breakeven possible - price <= variable cost */
    return -1.0; /* Error indicator */
  }

  be->quantity = be->fixedCost / contribution;
  return be->quantity;
}

double breakeven_calc_profit(Breakeven *be) {
  /*
   * Profit = Revenue - Total Cost
   *        = Q * P - (FC + Q * VC)
   *        = Q * (P - VC) - FC
   */
  double contribution = be->pricePerUnit - be->variableCostPerUnit;
  be->profit = be->quantity * contribution - be->fixedCost;
  return be->profit;
}

double breakeven_quantity_for_profit(Breakeven *be, double targetProfit) {
  /*
   * Target Profit = Q * (P - VC) - FC
   * Q = (FC + Target Profit) / (P - VC)
   */
  double contribution = be->pricePerUnit - be->variableCostPerUnit;

  if (contribution <= 0.0) {
    return -1.0; /* Error */
  }

  return (be->fixedCost + targetProfit) / contribution;
}

double breakeven_calc_revenue(Breakeven *be) {
  be->revenue = be->quantity * be->pricePerUnit;
  return be->revenue;
}

/* ============================================================
 * Cost-Sell-Margin
 * ============================================================ */

void margin_init(ProfitMargin *pm) {
  pm->cost = 0.0;
  pm->sellingPrice = 0.0;
  pm->margin = 0.0;
  pm->markup = 0.0;
}

double margin_calc_margin(ProfitMargin *pm) {
  /*
   * Margin (Gross Profit Margin):
   * Margin% = (Selling - Cost) / Selling * 100
   */
  if (pm->sellingPrice == 0.0)
    return 0.0;

  pm->margin = (pm->sellingPrice - pm->cost) / pm->sellingPrice * 100.0;
  return pm->margin;
}

double margin_calc_markup(ProfitMargin *pm) {
  /*
   * Markup:
   * Markup% = (Selling - Cost) / Cost * 100
   */
  if (pm->cost == 0.0)
    return 0.0;

  pm->markup = (pm->sellingPrice - pm->cost) / pm->cost * 100.0;
  return pm->markup;
}

double margin_calc_sell_from_margin(ProfitMargin *pm) {
  /*
   * Selling = Cost / (1 - Margin/100)
   */
  if (pm->margin >= 100.0)
    return 0.0; /* Invalid margin */

  pm->sellingPrice = pm->cost / (1.0 - pm->margin / 100.0);
  return pm->sellingPrice;
}

double margin_calc_sell_from_markup(ProfitMargin *pm) {
  /*
   * Selling = Cost * (1 + Markup/100)
   */
  pm->sellingPrice = pm->cost * (1.0 + pm->markup / 100.0);
  return pm->sellingPrice;
}

double margin_calc_cost_from_margin(ProfitMargin *pm) {
  /*
   * Cost = Selling * (1 - Margin/100)
   */
  pm->cost = pm->sellingPrice * (1.0 - pm->margin / 100.0);
  return pm->cost;
}

double margin_calc_cost_from_markup(ProfitMargin *pm) {
  /*
   * Cost = Selling / (1 + Markup/100)
   */
  if (pm->markup <= -100.0)
    return 0.0; /* Invalid markup */

  pm->cost = pm->sellingPrice / (1.0 + pm->markup / 100.0);
  return pm->cost;
}

/* ============================================================
 * Percent Calculations
 * ============================================================ */

double percent_change(double oldValue, double newValue) {
  /*
   * Percent Change = (New - Old) / Old * 100
   */
  if (oldValue == 0.0) {
    if (newValue == 0.0)
      return 0.0;
    return (newValue > 0) ? INFINITY : -INFINITY;
  }

  return (newValue - oldValue) / oldValue * 100.0;
}

double percent_difference(double val1, double val2) {
  /*
   * Percent Difference = |val1 - val2| / ((val1 + val2) / 2) * 100
   */
  double avg = (val1 + val2) / 2.0;

  if (avg == 0.0)
    return 0.0;

  double diff = val1 - val2;
  if (diff < 0)
    diff = -diff; /* abs */

  return diff / avg * 100.0;
}

double percent_of_total(double part, double total) {
  if (total == 0.0)
    return 0.0;
  return (part / total) * 100.0;
}

double value_from_percent(double total, double percent) {
  return total * (percent / 100.0);
}

double add_percent(double value, double percent) {
  return value * (1.0 + percent / 100.0);
}

double subtract_percent(double value, double percent) {
  return value * (1.0 - percent / 100.0);
}
