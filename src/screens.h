/**
 * Open fx-BA: TI BA II Plus Clone
 * screens.h - TI-style screen definitions and worksheets
 *
 * Defines all the different screens/worksheets that mimic TI BA II Plus:
 * - TVM Screen (main)
 * - Cash Flow Worksheet
 * - Bond Worksheet
 * - Depreciation Worksheet
 * - Statistics Worksheet
 */

#ifndef SCREENS_H
#define SCREENS_H

#include "types.h"

/* ============================================================
 * Screen/Worksheet Types
 * ============================================================ */
typedef enum {
  SCREEN_TVM,           /* Main TVM screen (N, I/Y, PV, PMT, FV) */
  SCREEN_CASH_FLOW,     /* Cash Flow worksheet */
  SCREEN_BOND,          /* Bond worksheet */
  SCREEN_DEPRECIATION,  /* Depreciation worksheet */
  SCREEN_AMORT,         /* Amortization worksheet */
  SCREEN_STATISTICS,    /* Statistics worksheet */
  SCREEN_DATE,          /* Date calculation */
  SCREEN_BREAKEVEN,     /* Breakeven analysis (Pro only) */
  SCREEN_PROFIT_MARGIN, /* Profit margin (Pro only) */
  SCREEN_MEMORY,        /* Memory operations */
  SCREEN_SETTINGS       /* P/Y, C/Y, BGN/END settings */
} ScreenType;

/* ============================================================
 * F-Key Menu Definitions
 * ============================================================ */

/* Maximum menu items per screen */
#define MAX_MENU_ITEMS 6

typedef struct {
  char label[8];    /* Short label for F-key (max 6 chars + null) */
  char label2nd[8]; /* Label when 2nd/SHIFT is pressed */
  int action;       /* Action code */
  int requires2nd;  /* 1 if this is a 2nd function */
} FKeyItem;

typedef struct {
  FKeyItem items[MAX_MENU_ITEMS];
  int count;
  int hasPage2; /* 1 if there are more items on next page */
} FKeyMenu;

/* ============================================================
 * Worksheet Entry Structure
 * ============================================================ */
typedef struct {
  char label[12];   /* Variable name (e.g., "N", "I/Y") */
  double *valuePtr; /* Pointer to the value */
  int editable;     /* 1 if user can edit */
  int computed;     /* 1 if this was just computed (show indicator) */
} WorksheetEntry;

/* ============================================================
 * Screen State
 * ============================================================ */
typedef struct {
  ScreenType currentScreen;
  int menuPage;      /* For screens with multiple menu pages */
  int selectedEntry; /* For worksheet mode: which entry is selected */
  int is2ndActive;   /* 1 if 2nd/SHIFT was pressed */
  int isCPTActive;   /* 1 if CPT was pressed */
} ScreenState;

/* ============================================================
 * Screen Drawing Functions
 * ============================================================ */

/**
 * Initialize screen state
 */
void screen_init(ScreenState *state);

/**
 * Draw the complete screen based on current state
 */
void screen_draw(Calculator *calc, ScreenState *state);

/**
 * Draw TVM main screen (TI-style single line display)
 */
void screen_draw_tvm(Calculator *calc, ScreenState *state);

/**
 * Draw worksheet-style screen (list of variables)
 */
void screen_draw_worksheet(Calculator *calc, ScreenState *state,
                           WorksheetEntry entries[], int count);

/**
 * Draw F-key menu at bottom
 */
void draw_fkey_menu(FKeyMenu *menu, int is2ndActive);

/* ============================================================
 * Pre-defined Menus
 * ============================================================ */

/**
 * Get the main TVM menu
 */
FKeyMenu *get_tvm_menu(void);

/**
 * Get the cash flow menu
 */
FKeyMenu *get_cashflow_menu(void);

/**
 * Get the bond menu
 */
FKeyMenu *get_bond_menu(void);

/**
 * Get the depreciation menu
 */
FKeyMenu *get_depreciation_menu(void);

#endif /* SCREENS_H */
