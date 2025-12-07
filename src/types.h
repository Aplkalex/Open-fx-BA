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
    MODEL_STANDARD,      /* TI BA II Plus */
    MODEL_PROFESSIONAL   /* TI BA II Plus Professional */
} CalculatorModel;

/* ============================================================
 * Application State (State Machine)
 * ============================================================ */
typedef enum {
    STATE_INPUT,      /* User is entering digits */
    STATE_COMPUTE,    /* CPT pressed, waiting for variable key */
    STATE_RESULT,     /* Displaying calculation result */
    STATE_ERROR,      /* Error state */
    STATE_WAIT_STO,   /* STO pressed, waiting for 0-9 */
    STATE_WAIT_RCL    /* RCL pressed, waiting for 0-9 */
} AppState;

/* ============================================================
 * TVM Variables Mode (END vs BEGIN)
 * ============================================================ */
typedef enum {
    TVM_END = 0,    /* Ordinary annuity (payments at end of period) */
    TVM_BEGIN = 1   /* Annuity due (payments at beginning of period) */
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
    double N;       /* Number of periods */
    double I_Y;     /* Interest rate per year (%) */
    double PV;      /* Present Value */
    double PMT;     /* Payment per period */
    double FV;      /* Future Value */
    double P_Y;     /* Payments per year */
    double C_Y;     /* Compounding periods per year */
    TVMMode mode;   /* END or BEGIN */
} TVM_Data;

/* ============================================================
 * Cash Flow Item (for NPV/IRR calculations)
 * ============================================================ */
typedef struct {
    double amount;      /* Cash flow amount */
    int frequency;      /* Number of times this flow repeats (F01, F02, etc.) */
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
    
    /* UI State */
    AppState state;
    char inputBuffer[INPUT_BUFFER_SIZE];
    int inputLength;
    int hasDecimal;
    int isNegative;
    
    /* Status flags for UI indicators */
    int is2ndActive;      /* 2ND key pressed */
    int isComputeActive;  /* CPT key pressed */
    
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
#define ERR_NONE            0
#define ERR_NO_SOLUTION     1   /* No solution exists */
#define ERR_OVERFLOW        2   /* Number too large */
#define ERR_ITERATION       3   /* Iteration limit exceeded */
#define ERR_INVALID_INPUT   4   /* Invalid input */
#define ERR_IRR_MULTIPLE    5   /* Multiple IRR solutions */

#endif /* TYPES_H */
