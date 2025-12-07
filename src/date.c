/**
 * Open fx-BA: TI BA II Plus Clone
 * date.c - Date calculations implementation
 *
 * Implements TI BA II Plus Date worksheet functionality:
 * - ACT (actual day count)
 * - 360 (30/360 day count convention)
 * - Date arithmetic
 */

#include "date.h"
#include <stdlib.h>

/* ============================================================
 * Constants
 * ============================================================ */

static const int DAYS_IN_MONTH[] = {0,  31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};

static const char *DAY_NAMES[] = {"SUN", "MON", "TUE", "WED",
                                  "THU", "FRI", "SAT"};

/* Reference epoch: January 1, 1900 was a Monday */
#define EPOCH_YEAR 1900

/* ============================================================
 * Basic Date Functions
 * ============================================================ */

int date_is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int date_days_in_month(int month, int year) {
  if (month < 1 || month > 12)
    return 0;

  if (month == 2 && date_is_leap_year(year)) {
    return 29;
  }
  return DAYS_IN_MONTH[month];
}

int date_is_valid(Date *d) {
  if (d->year < 1900 || d->year > 2099)
    return 0;
  if (d->month < 1 || d->month > 12)
    return 0;
  if (d->day < 1)
    return 0;

  int maxDay = date_days_in_month(d->month, d->year);
  if (d->day > maxDay)
    return 0;

  return 1;
}

/* ============================================================
 * Days Since Epoch Conversion
 * ============================================================ */

long date_to_days_since_epoch(Date *d) {
  long days = 0;

  /* Add days for complete years */
  for (int y = EPOCH_YEAR; y < d->year; y++) {
    days += date_is_leap_year(y) ? 366 : 365;
  }

  /* Add days for complete months in current year */
  for (int m = 1; m < d->month; m++) {
    days += date_days_in_month(m, d->year);
  }

  /* Add days in current month */
  days += d->day;

  return days;
}

void date_from_days_since_epoch(long days, Date *result) {
  if (days < 1) {
    result->year = EPOCH_YEAR;
    result->month = 1;
    result->day = 1;
    return;
  }

  int year = EPOCH_YEAR;

  /* Find the year */
  while (1) {
    int daysInYear = date_is_leap_year(year) ? 366 : 365;
    if (days <= daysInYear)
      break;
    days -= daysInYear;
    year++;
  }

  /* Find the month */
  int month = 1;
  while (month <= 12) {
    int daysInMonth = date_days_in_month(month, year);
    if (days <= daysInMonth)
      break;
    days -= daysInMonth;
    month++;
  }

  result->year = year;
  result->month = month;
  result->day = (int)days;
}

/* ============================================================
 * Date Difference (ACT and 360 modes)
 * ============================================================ */

int date_diff(Date *d1, Date *d2, DateMode mode) {
  if (mode == DATE_MODE_360) {
    /* 30/360 convention (US) */
    int y1 = d1->year, m1 = d1->month, day1 = d1->day;
    int y2 = d2->year, m2 = d2->month, day2 = d2->day;

    /* Adjustment rules for 30/360 */
    if (day1 == 31)
      day1 = 30;
    if (day2 == 31 && day1 >= 30)
      day2 = 30;

    /* Handle February end-of-month */
    int febDays1 = date_days_in_month(2, y1);
    int febDays2 = date_days_in_month(2, y2);

    if (m1 == 2 && day1 == febDays1)
      day1 = 30;
    if (m2 == 2 && day2 == febDays2 && day1 == 30)
      day2 = 30;

    return 360 * (y2 - y1) + 30 * (m2 - m1) + (day2 - day1);
  }

  /* ACT mode: actual day count */
  long days1 = date_to_days_since_epoch(d1);
  long days2 = date_to_days_since_epoch(d2);

  return (int)(days2 - days1);
}

/* ============================================================
 * Date Arithmetic
 * ============================================================ */

void date_add_days(Date *d, int days, Date *result) {
  long totalDays = date_to_days_since_epoch(d) + days;
  date_from_days_since_epoch(totalDays, result);
}

/* ============================================================
 * Day of Week
 * ============================================================ */

int date_day_of_week(Date *d) {
  /* January 1, 1900 was a Monday (day 1) */
  long days = date_to_days_since_epoch(d);

  /* days = 1 is Monday, so (days - 1) % 7 gives 0=Mon, 1=Tue, etc. */
  /* We want 0=Sun, 1=Mon, etc. */
  int dow = ((days - 1) % 7) + 1; /* 1=Mon ... 7=Sun */
  dow = dow % 7;                  /* Convert to 0=Sun ... 6=Sat */

  return dow;
}

const char *date_day_name(int dayOfWeek) {
  if (dayOfWeek < 0 || dayOfWeek > 6)
    return "???";
  return DAY_NAMES[dayOfWeek];
}

/* ============================================================
 * Date Parsing (TI Format: MM.DDYYYY or DD.MMYYYY)
 * ============================================================ */

int date_parse(double value, DateFormat format, Date *result) {
  /* Extract parts from format like 12.252024 */
  int firstPart = (int)value; /* 12 */

  /* Get fractional part and convert to integer */
  double frac = value - (double)firstPart;
  long fracInt = (long)(frac * 1000000.0 + 0.5); /* 252024 */

  int secondPart = (int)(fracInt / 10000); /* 25 */
  int year = (int)(fracInt % 10000);       /* 2024 */

  if (format == DATE_FORMAT_US) {
    /* MM.DDYYYY */
    result->month = firstPart;
    result->day = secondPart;
  } else {
    /* DD.MMYYYY */
    result->day = firstPart;
    result->month = secondPart;
  }
  result->year = year;

  return date_is_valid(result);
}

double date_format(Date *d, DateFormat format) {
  int firstPart, secondPart;

  if (format == DATE_FORMAT_US) {
    firstPart = d->month;
    secondPart = d->day;
  } else {
    firstPart = d->day;
    secondPart = d->month;
  }

  /* Build the fractional part: DDYYYY or MMYYYY */
  long fracInt = (long)secondPart * 10000 + d->year;

  return (double)firstPart + (double)fracInt / 1000000.0;
}

/* ============================================================
 * Date Worksheet Functions
 * ============================================================ */

void date_worksheet_init(DateWorksheet *ws) {
  /* Default to today's date (placeholder: Jan 1, 2024) */
  ws->dt1.year = 2024;
  ws->dt1.month = 1;
  ws->dt1.day = 1;

  ws->dt2.year = 2024;
  ws->dt2.month = 12;
  ws->dt2.day = 31;

  ws->dbdDays = 0;
  ws->mode = DATE_MODE_ACT;
  ws->format = DATE_FORMAT_US;
}

void date_worksheet_compute_dbd(DateWorksheet *ws) {
  ws->dbdDays = date_diff(&ws->dt1, &ws->dt2, ws->mode);
}

void date_worksheet_compute_dt2(DateWorksheet *ws) {
  date_add_days(&ws->dt1, ws->dbdDays, &ws->dt2);
}

void date_worksheet_compute_dt1(DateWorksheet *ws) {
  date_add_days(&ws->dt2, -ws->dbdDays, &ws->dt1);
}
