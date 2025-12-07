/**
 * Open fx-BA: TI BA II Plus Clone
 * profit.h - Breakeven, Cost-Sell-Margin, Percent calculations
 */

#ifndef PROFIT_H
#define PROFIT_H

/* ============================================================
 * Breakeven Analysis
 * ============================================================ */
typedef struct {
  double fixedCost;           /* FC: Fixed costs */
  double variableCostPerUnit; /* VC: Variable cost per unit */
  double pricePerUnit;        /* P: Selling price per unit */
  double quantity;            /* Q: Quantity (breakeven or actual) */
  double profit;              /* PFT: Profit (or loss if negative) */
  double revenue;             /* REV: Total revenue */
} Breakeven;

/**
 * Initialize breakeven structure with defaults.
 */
void breakeven_init(Breakeven *be);

/**
 * Calculate breakeven quantity.
 * Q = FC / (P - VC)
 */
double breakeven_calc_quantity(Breakeven *be);

/**
 * Calculate profit at given quantity.
 * PFT = Q * (P - VC) - FC
 */
double breakeven_calc_profit(Breakeven *be);

/**
 * Calculate quantity needed for target profit.
 * Q = (FC + Target Profit) / (P - VC)
 */
double breakeven_quantity_for_profit(Breakeven *be, double targetProfit);

/**
 * Calculate revenue at given quantity.
 */
double breakeven_calc_revenue(Breakeven *be);

/* ============================================================
 * Cost-Sell-Margin (Pro Only)
 * ============================================================ */
typedef struct {
  double cost;         /* CST: Cost price */
  double sellingPrice; /* SEL: Selling price */
  double margin;       /* MAR: Margin percentage (profit/selling) */
  double markup;       /* MU: Markup percentage (profit/cost) */
} ProfitMargin;

/**
 * Initialize profit margin with defaults.
 */
void margin_init(ProfitMargin *pm);

/**
 * Calculate margin from cost and selling price.
 * MAR = (SEL - CST) / SEL * 100
 */
double margin_calc_margin(ProfitMargin *pm);

/**
 * Calculate markup from cost and selling price.
 * MU = (SEL - CST) / CST * 100
 */
double margin_calc_markup(ProfitMargin *pm);

/**
 * Calculate selling price from cost and margin.
 * SEL = CST / (1 - MAR/100)
 */
double margin_calc_sell_from_margin(ProfitMargin *pm);

/**
 * Calculate selling price from cost and markup.
 * SEL = CST * (1 + MU/100)
 */
double margin_calc_sell_from_markup(ProfitMargin *pm);

/**
 * Calculate cost from selling price and margin.
 * CST = SEL * (1 - MAR/100)
 */
double margin_calc_cost_from_margin(ProfitMargin *pm);

/**
 * Calculate cost from selling price and markup.
 * CST = SEL / (1 + MU/100)
 */
double margin_calc_cost_from_markup(ProfitMargin *pm);

/* ============================================================
 * Percent Calculations
 * ============================================================ */

/**
 * Calculate percent change.
 * %CH = (new - old) / old * 100
 */
double percent_change(double oldValue, double newValue);

/**
 * Calculate percent difference.
 * %DIFF = |val1 - val2| / ((val1 + val2) / 2) * 100
 */
double percent_difference(double val1, double val2);

/**
 * Calculate percent of total.
 * Return = (part / total) * 100
 */
double percent_of_total(double part, double total);

/**
 * Calculate value from percent.
 * Return = total * (percent / 100)
 */
double value_from_percent(double total, double percent);

/**
 * Add percent to value.
 * Return = value * (1 + percent/100)
 */
double add_percent(double value, double percent);

/**
 * Subtract percent from value.
 * Return = value * (1 - percent/100)
 */
double subtract_percent(double value, double percent);

#endif /* PROFIT_H */
