/**
 * Open fx-BA: TI BA II Plus Clone
 * input.h - Input handling and state machine
 */

#ifndef INPUT_H
#define INPUT_H

#include "types.h"

/* ============================================================
 * Calculator Initialization
 * ============================================================ */
void calc_init(Calculator *calc, CalculatorModel model);
void calc_reset_tvm(Calculator *calc);
void calc_reset_cashflow(Calculator *calc);

/* ============================================================
 * Input Buffer Management
 * ============================================================ */
void input_clear(Calculator *calc);
void input_append_digit(Calculator *calc, char digit);
void input_append_decimal(Calculator *calc);
void input_toggle_negative(Calculator *calc);
void input_backspace(Calculator *calc);
double input_get_value(Calculator *calc);

/* ============================================================
 * State Machine
 * ============================================================ */
void state_set(Calculator *calc, AppState newState);
void state_handle_var_key(Calculator *calc, TVMVariable var);
void state_handle_cpt_key(Calculator *calc);

/* ============================================================
 * STO/RCL Memory State Handling
 * ============================================================ */
void state_handle_sto_key(Calculator *calc);
void state_handle_rcl_key(Calculator *calc);
void state_handle_memory_digit(Calculator *calc, int digit);
void state_check_timeout(Calculator *calc);
void state_cancel_sto_rcl(Calculator *calc);

/* ============================================================
 * Error Handling (TI BA II Plus style)
 * ============================================================ */
void error_set(Calculator *calc, int errorCode, const char *message);
void error_clear(Calculator *calc);
int error_is_active(Calculator *calc);

/* ============================================================
 * TVM Variable Access
 * ============================================================ */
void tvm_store(Calculator *calc, TVMVariable var, double value);
double tvm_recall(Calculator *calc, TVMVariable var);

#endif /* INPUT_H */
