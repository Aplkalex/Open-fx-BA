# Open fx-BA: TI BA II Plus Clone for Casio fx-9750GIII

[![Tests](https://img.shields.io/badge/tests-38%2F38-brightgreen)](.) [![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

An open source add-in for Casio fx-9750GIII / fx-9860GIII / fx-9860G series, implementing **TI BA II Plus Standard & Professional** financial calculation functions.

## Features

### Standard Mode
| Feature | Description |
|---------|-------------|
| **TVM** | N, I/Y, PV, PMT, FV with P/Y, C/Y, BGN/END |
| **Cash Flow** | NPV, IRR |
| **Amortization** | P1, P2, BAL, PRN, INT |
| **Bond** | Price, Yield, Accrued Interest |
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
