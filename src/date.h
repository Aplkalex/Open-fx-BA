/**
 * Open fx-BA: TI BA II Plus Clone
 * date.h - Date calculations (matching TI BA II Plus Date worksheet)
 *
 * Features:
 * - Days between dates
 * - Date + days = new date
 * - Day count conventions (ACT, 360)
 * - Day of week calculation
 */

#ifndef DATE_H
#define DATE_H

/* ============================================================
 * Date Format Modes
 * ============================================================ */
typedef enum {
  DATE_FORMAT_US, /* MM.DDYYYY (US style) */
  DATE_FORMAT_EUR /* DD.MMYYYY (European style) */
} DateFormat;

/* ============================================================
 * Day Count Modes (for date difference calculations)
 * ============================================================ */
typedef enum {
  DATE_MODE_ACT, /* Actual days (365 or 366 per year) */
  DATE_MODE_360  /* 30/360 (assumes 30 days per month) */
} DateMode;

/* ============================================================
 * Date Structure
 * ============================================================ */
typedef struct {
  int year;  /* 4-digit year (1900-2099) */
  int month; /* 1-12 */
  int day;   /* 1-31 */
} Date;

/* ============================================================
 * Date Calculation Functions
 * ============================================================ */

/**
 * Validate a date.
 * @return 1 if valid, 0 if invalid
 */
int date_is_valid(Date *d);

/**
 * Check if year is a leap year.
 */
int date_is_leap_year(int year);

/**
 * Get number of days in a month.
 */
int date_days_in_month(int month, int year);

/**
 * Calculate days between two dates.
 *
 * @param d1 First date
 * @param d2 Second date
 * @param mode ACT (actual) or 360 (30/360 convention)
 * @return Number of days (positive if d2 > d1)
 */
int date_diff(Date *d1, Date *d2, DateMode mode);

/**
 * Add days to a date.
 *
 * @param d Starting date
 * @param days Number of days to add (can be negative)
 * @param result Output: resulting date
 */
void date_add_days(Date *d, int days, Date *result);

/**
 * Calculate day of week.
 *
 * @param d Date to check
 * @return 0=Sunday, 1=Monday, ..., 6=Saturday
 */
int date_day_of_week(Date *d);

/**
 * Get day of week name.
 */
const char *date_day_name(int dayOfWeek);

/**
 * Convert date to days since epoch (for internal calculations).
 * Epoch: January 1, 1900
 */
long date_to_days_since_epoch(Date *d);

/**
 * Convert days since epoch to date.
 */
void date_from_days_since_epoch(long days, Date *result);

/**
 * Parse a date from TI-style format (MM.DDYYYY or DD.MMYYYY)
 *
 * @param value The numeric value (e.g., 12.252024 for Dec 25, 2024)
 * @param format US or EUR
 * @param result Output: parsed date
 * @return 1 if successful, 0 if invalid
 */
int date_parse(double value, DateFormat format, Date *result);

/**
 * Format a date to TI-style format.
 *
 * @param d Date to format
 * @param format US or EUR
 * @return Numeric value (e.g., 12.252024)
 */
double date_format(Date *d, DateFormat format);

/* ============================================================
 * Date Worksheet Data (TI-style)
 * ============================================================ */
typedef struct {
  Date dt1;          /* First date (DT1) */
  Date dt2;          /* Second date (DT2) */
  int dbdDays;       /* Days between dates (DBD) */
  DateMode mode;     /* ACT or 360 */
  DateFormat format; /* US or EUR display format */
} DateWorksheet;

/**
 * Initialize date worksheet with defaults.
 */
void date_worksheet_init(DateWorksheet *ws);

/**
 * Compute DBD (days between dates) from DT1 and DT2.
 */
void date_worksheet_compute_dbd(DateWorksheet *ws);

/**
 * Compute DT2 from DT1 and DBD.
 */
void date_worksheet_compute_dt2(DateWorksheet *ws);

/**
 * Compute DT1 from DT2 and DBD.
 */
void date_worksheet_compute_dt1(DateWorksheet *ws);

#endif /* DATE_H */
