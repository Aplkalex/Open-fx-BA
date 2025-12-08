# Open fx-BA: TI BA II Plus Clone for Casio fx-9750GIII

![Release](https://img.shields.io/badge/RELEASE-BETA_TBD-EBF2F9?style=for-the-badge&labelColor=333)
![License](https://img.shields.io/badge/LICENSE-MIT-50799E?style=for-the-badge&labelColor=333)
![Status](https://img.shields.io/badge/STATUS-ACTIVE-A9BD3B?style=for-the-badge&labelColor=333)

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![fxSDK](https://img.shields.io/badge/fxSDK-CASIO_SDK-4a4a4a?style=for-the-badge)
![Tests](https://img.shields.io/badge/TESTS-39%2F39_PASSING-success?style=for-the-badge)

![Compatible](https://img.shields.io/badge/COMPATIBLE_WITH-CASIO_FX--9860G_SERIES-fffcf2?style=for-the-badge&labelColor=333)

An open source add-in for Casio fx-9750GIII / fx-9860GIII / fx-9860G series, implementing **TI BA II Plus Standard & Professional** financial calculation functions.

## Features

### Standard Mode
| Feature | Description |
|---------|-------------|
| **TVM** | N, I/Y, PV, PMT, FV with P/Y, C/Y, BGN/END |
| **Cash Flow** | NPV, IRR |
| **Amortization** | P1, P2, BAL, PRN, INT |
| **Bond** | Price, Yield (YTM/YTC), Accrued Interest, Callable bonds |
| **Depreciation** | Straight Line (SL), Sum-of-Years (SYD) |
| **Date** | Days between dates (ACT and 30/360) |
| **Statistics** | 1-var, 2-var, Linear Regression |
| **Memory** | STO/RCL with M0-M9 |

### Professional Mode (Additional)
| Feature | Description |
|---------|-------------|
| **Cash Flow+** | NFV, MIRR, Payback, Discounted Payback |
| **Bond+** | Duration, Modified Duration |
| **Depreciation+** | DB, DB-SL crossover (6 methods total) |
| **Breakeven** | FC, VC, P, Q, PFT analysis |
| **Profit Margin** | Cost, Selling, Margin%, Markup% |
| **Statistics+** | 4 regression types, Forecasting |

## Specification Coverage

### Standard Mode Specification
- Time-Value-of-Money worksheet with P/Y, C/Y, BEGIN/END, amortization schedules, and annuity/mortgage/lease/saving scenarios  
- Cash-flow worksheet supporting the TI-spec 24 uneven flows (32 internally) with four-digit frequencies, computing NPV and IRR  
- Bond worksheet with Actual/Actual or 30/360 day-count methods for clean price, yield-to-maturity or yield-to-call, and accrued interest  
- Depreciation schedules with SL, SYD, declining-balance, and DB with straight-line crossover, including book value and remaining depreciable amount plus partial-year handling  
- Date worksheet (days between dates, partial years), BGN/END payment setting, ten-digit display, and ten user memories  
- List-based one- and two-variable statistics with linear, logarithmic, exponential, and power regression options  
- Prompted TI-style display showing the active variable/label, 2ND/CPT indicators, and percent difference/change functions (general trig/log/power math uses the fx-9750GIII OS)

### Professional Mode Specification
- All Standard features plus cash-flow NFV, MIRR, payback, discounted payback, and cash-flow forecasting  
- Bond duration and modified duration calculations, including yield-to-call mode  
- Depreciation DB, DB with SL crossover, French SLF/DBF methods, and full depreciation schedules  
- Breakeven, profit, target profit, percent difference/change, cost/sell/margin/markup worksheets  
- Extended statistics and forecasting outputs, plus 2ND-key shortcuts matching TI BA II Plus Professional behavior  

## Quick Start

### Key Mappings (TI-style)
| Key | Function |
|-----|----------|
| SHIFT | 2ND mode toggle |
| SHIFT + F3 | AMORT |
| SHIFT + F4 | BGN/END toggle |
| SHIFT + 7 | Cash Flow |
| SHIFT + 8 | Bond |
| SHIFT + 9 | Depreciation |
| SHIFT + 4 | Statistics |
| SHIFT + 5 | Date |
| SHIFT + 6 | Breakeven (Pro) |
| UP/DOWN | Navigate worksheet |
| EXIT | Return to TVM |

## Building

### Prerequisites
```bash
# macOS (via Homebrew)
brew tap Lephenixnoir/fxsdk
brew install fxsdk

# Or: https://gitea.planet-casio.com/Lephenixnoir/fxsdk
```

### Compile & Test
```bash
# Run test suite (38 CFA-style tests)
make cfa-test

# Build for calculator
fxsdk build-fx
```

### Install
1. Connect fx-9750GIII via USB
2. Copy `Open-fx-BA.g3a` to calculator
3. Open from Main Menu

## Test Coverage

**38/38 tests passing** including:
- 10 CFA Level I-III exam-style questions
- 10 Advanced financial calculations
- 5 Worksheet integration tests
- 5 Edge case boundary tests
- 8 Additional worksheet tests (amort, breakeven, margin, date, regression)

## Project Structure
```
src/
├── main.c          # Entry point & event loop
├── tvm.c/h         # TVM solver & amortization
├── cashflow.c/h    # NPV, IRR, NFV
├── bond.c/h        # Bond pricing & duration
├── depreciation.c/h # 6 depreciation methods
├── statistics.c/h  # Stats & regression
├── date.c/h        # Date calculations
├── profit.c/h      # Breakeven & margin
├── features.c/h    # Pro/Standard gating
├── display.c/h     # TI-style display
└── tests.c/h       # CFA validation suite
```

## License

MIT License
