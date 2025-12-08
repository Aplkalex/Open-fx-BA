/**
 * Open fx-BA: TI BA II Plus Clone
 * types.h - Core data structures
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* Forward declaration */
#include "memory.h"

/* ============================================================
 * Calculator Model (Standard vs Professional)
 * ============================================================ */
typedef enum {
  MODEL_STANDARD,    /* TI BA II Plus */
  MODEL_PROFESSIONAL /* TI BA II Plus Professional */
} CalculatorModel;

/* ============================================================
 * Application State (State Machine)
 * ============================================================ */
typedef enum {
  STATE_INPUT,    /* User is entering digits */
  STATE_COMPUTE,  /* CPT pressed, waiting for variable key */
  STATE_RESULT,   /* Displaying calculation result */
  STATE_ERROR,    /* Error state */
  STATE_WAIT_STO, /* STO pressed, waiting for 0-9 */
  STATE_WAIT_RCL  /* RCL pressed, waiting for 0-9 */
} AppState;

/* ============================================================
 * TVM Variables Mode (END vs BEGIN)
 * ============================================================ */
typedef enum {
  TVM_END = 0,  /* Ordinary annuity (payments at end of period) */
  TVM_BEGIN = 1 /* Annuity due (payments at beginning of period) */
} TVMMode;

/* ============================================================
 * TVM Variable Index
 * ============================================================ */
typedef enum {
  TVM_VAR_N = 0,
  TVM_VAR_IY,
  TVM_VAR_PV,
  TVM_VAR_PMT,
  TVM_VAR_FV,
  TVM_VAR_COUNT
} TVMVariable;

/* ============================================================
 * TVM Data Structure
 * ============================================================ */
typedef struct {
  double N;     /* Number of periods */
  double I_Y;   /* Interest rate per year (%) */
  double PV;    /* Present Value */
  double PMT;   /* Payment per period */
  double FV;    /* Future Value */
  double P_Y;   /* Payments per year */
  double C_Y;   /* Compounding periods per year */
  TVMMode mode; /* END or BEGIN */
} TVM_Data;

/* ============================================================
 * Cash Flow Item (for NPV/IRR calculations)
 * ============================================================ */
typedef struct {
  double amount; /* Cash flow amount */
  int frequency; /* Number of times this flow repeats (F01, F02, etc.) */
} CashFlowItem;

/* ============================================================
 * Cash Flow List
 * ============================================================ */
#define MAX_CASH_FLOWS 32

typedef struct {
  double CF0;                         /* Initial cash flow (usually negative) */
  CashFlowItem flows[MAX_CASH_FLOWS]; /* Subsequent cash flows */
  int count;                          /* Number of cash flow groups */
} CashFlowList;

/* ============================================================
 * Forward declarations for worksheet data
 * (Full definitions in respective headers)
 * ============================================================ */

/* Bond worksheet data */
typedef struct {
  int settlementDate; /* Settlement date YYYYMMDD */
  double couponRate;  /* Coupon rate (%) */
  int maturityDate;   /* Maturity date YYYYMMDD */
  int callDate;       /* Call date YYYYMMDD (0 if non-callable) */
  double callPrice;   /* Call price (% of par) */
  double redemption;  /* Redemption value */
  double price;       /* Computed or input */
  double yield;       /* Computed or input */
  int bondType;       /* 0=YTM, 1=YTC */
} BondData;

/* Depreciation worksheet data */
typedef struct {
  double cost;     /* Original cost */
  double salvage;  /* Salvage value */
  double life;     /* Life in years */
  double dbRate;   /* DB rate (e.g., 200 for DDB) */
  int startMonth;  /* 1-12 */
  int method;      /* 0=SL, 1=SYD, 2=DB, 3=DB-SL, 4=SLF, 5=DBF */
  int currentYear; /* Year being viewed */
} DeprData;

/* Date worksheet data */
typedef struct {
  int dt1;  /* Date 1 as YYYYMMDD */
  int dt2;  /* Date 2 as YYYYMMDD */
  int dbd;  /* Days between dates */
  int mode; /* 0=ACT, 1=360 */
} DateData;

/* Statistics data */
typedef struct {
  double xData[50];
  double yData[50];
  int count;
  int regType; /* 0=LIN, 1=LOG, 2=EXP, 3=PWR */
} StatDataSimple;

/* Breakeven analysis data */
typedef struct {
  double fixedCost;    /* FC: Fixed costs */
  double variableCost; /* VC: Variable cost per unit */
  double price;        /* P: Selling price per unit */
  double quantity;     /* Q: Quantity */
  double profit;       /* PFT: Profit */
} BreakevenData;

/* Profit margin data */
typedef struct {
  double cost;    /* CST: Cost price */
  double selling; /* SEL: Selling price */
  double margin;  /* MAR: Margin % */
  double markup;  /* MU: Markup % */
} ProfitMarginData;

/* Amortization worksheet state */
typedef struct {
  int p1;           /* Starting period */
  int p2;           /* Ending period */
  double balance;   /* BAL: Remaining balance */
  double principal; /* PRN: Principal paid */
  double interest;  /* INT: Interest paid */
  int currentField; /* 0=P1, 1=P2, 2=BAL, 3=PRN, 4=INT */
} AmortState;

/* ============================================================
 * Calculator State (Global app state)
 * ============================================================ */
#define INPUT_BUFFER_SIZE 16

typedef struct {
  /* Model configuration */
  CalculatorModel model;

  /* Current TVM data */
  TVM_Data tvm;

  /* Cash flow data */
  CashFlowList cashflow;

  /* Memory registers (M0-M9) */
  MemoryRegisters memory;

  /* Worksheet data */
  BondData bond;
  DeprData depreciation;
  DateData dateWs;
  StatDataSimple statistics;
  BreakevenData breakeven;
  ProfitMarginData profitMargin;
  AmortState amort;

  /* UI State */
  AppState state;
  char inputBuffer[INPUT_BUFFER_SIZE];
  int inputLength;
  int hasDecimal;
  int isNegative;

  /* Status flags for UI indicators */
  int is2ndActive;     /* 2ND key pressed */
  int isComputeActive; /* CPT key pressed */

  /* Timeout counter for STO/RCL states */
  int stateTimeout;

  /* Current worksheet/screen */
  int currentScreen;

  /* Current worksheet variable index (for up/down navigation) */
  int worksheetIndex;

  /* Error code (if any) */
  int errorCode;

  /* Error message for display */
  char errorMessage[16];
} Calculator;

/* ============================================================
 * Error Codes (matching TI's error system)
 * ============================================================ */
#define ERR_NONE 0
#define ERR_NO_SOLUTION 1   /* No solution exists */
#define ERR_OVERFLOW 2      /* Number too large */
#define ERR_ITERATION 3     /* Iteration limit exceeded */
#define ERR_INVALID_INPUT 4 /* Invalid input */
#define ERR_IRR_MULTIPLE 5  /* Multiple IRR solutions */

#endif /* TYPES_H */
