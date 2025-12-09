<div align="center">

# Open fx-BA - TI BA II Plus for Casio fx-9750GIII / fx-9860GIII (fx-9860G)

**An open source TI BA II Plus add-in for Casio fx-9860G series.**


![Release](https://img.shields.io/badge/BETA%20RELEASE-8%20DEC%202025-EBF2F9?style=for-the-badge&labelColor=333)
![License](https://img.shields.io/badge/LICENSE-MIT-50799E?style=for-the-badge&labelColor=333)
![Status](https://img.shields.io/badge/STATUS-ACTIVE-A9BD3B?style=for-the-badge&labelColor=333)

![fx-9750GIII](https://img.shields.io/badge/COMPATIBLE-fx--9750GIII-d6ccc2?style=for-the-badge&labelColor=333)
![fx-9860GIII](https://img.shields.io/badge/COMPATIBLE-fx--9860GIII-f5ebe0?style=for-the-badge&labelColor=333)
![fx-9860G](https://img.shields.io/badge/COMPATIBLE-fx--9860G-e3d5ca?style=for-the-badge&labelColor=333)

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Tests](https://img.shields.io/badge/TESTS-39%2F39_PASSING-success?style=for-the-badge)



</div>
<br/>

---

## 📺 Display Layout

The calculator displays information in TI BA II Plus style:

```
┌────────────────────────────────────────┐
│ STD          BGN      2nd         D4   │ ← Status Bar
│────────────────────────────────────────│
│ N=                                     │ ← Variable Prompt
│                           1,234.56   * │ ← Value (right-aligned)
│                                        │
│ COMPUTE                                │ ← Indicator
│────────────────────────────────────────│
│  N    I/Y   PV   PMT   FV   CPT       │ ← F-Key Labels
└────────────────────────────────────────┘
```

### Status Bar Indicators
| Indicator | Meaning |
|-----------|---------|
| **STD/PRO** | Standard or Professional mode |
| **BGN** | Payments at beginning of period |
| **2nd** | 2nd function mode active (SHIFT pressed) |
| **D4** | Display format (4 decimal places) |
| **CF/AM/BD/DP/DT/ST/BE** | Current worksheet |

---

## ⌨️ F-Key Functions

### TVM Mode (Main Screen)

| Key | Normal Function | 2nd Function (SHIFT) |
|:---:|-----------------|---------------------|
| **F1** | **N** - Number of periods | **xP/Y** - Times P/Y |
| **F2** | **I/Y** - Interest rate/year | **P/Y** - Payments/year settings |
| **F3** | **PV** - Present value | **AMORT** - Amortization worksheet |
| **F4** | **PMT** - Payment amount | **BGN** - Toggle BEGIN/END mode |
| **F5** | **FV** - Future value | **CLR** - Clear TVM values |
| **F6** | **CPT** - Compute | **QUIT** - Return to TVM screen |

### Cash Flow Mode (SHIFT + 7)

| Key | Normal | 2nd |
|:---:|--------|-----|
| **F1** | CF0 - Initial cash flow | - |
| **F2** | CFn - Cash flow entry | F01 - Frequency |
| **F3** | NPV - Net present value | NFV - Net future value |
| **F4** | IRR - Internal rate of return | MIRR - Modified IRR |
| **F5** | - | INS - Insert flow |
| **F6** | ↓ - Next flow | CLR - Clear cash flows |

### Amortization Mode (SHIFT + F3)

| Key | Normal | 2nd |
|:---:|--------|-----|
| **F1** | P1 - Start period | - |
| **F2** | P2 - End period | - |
| **F3** | BAL - Remaining balance | - |
| **F4** | PRN - Principal paid | - |
| **F5** | INT - Interest paid | - |
| **F6** | ↓ - Navigate | - |

### Bond Mode (SHIFT + 8)

| Key | Normal | 2nd |
|:---:|--------|-----|
| **F1** | SDT - Settlement date | - |
| **F2** | CPN - Coupon rate | - |
| **F3** | RDT - Redemption date | - |
| **F4** | RV - Redemption value | AI - Accrued interest |
| **F5** | 360 - Day count mode | DUR - Duration |
| **F6** | ↓ - Navigate | SET - YTM/YTC toggle |

### Depreciation Mode (SHIFT + 9)

| Key | Normal | 2nd |
|:---:|--------|-----|
| **F1** | CST - Cost | M01 - Start month |
| **F2** | SAL - Salvage value | - |
| **F3** | LIF - Life in years | - |
| **F4** | YR - Year to compute | - |
| **F5** | DEP - Depreciation | RBV - Remaining book value |
| **F6** | ↓ - Navigate | - |

### Statistics Mode (SHIFT + 4)

| Key | Normal | 2nd |
|:---:|--------|-----|
| **F1** | X - Enter X data | - |
| **F2** | Y - Enter Y data | - |
| **F3** | n - Sample count | - |
| **F4** | x̄ - Mean of X | ȳ - Mean of Y |
| **F5** | Sx - Std dev X | Sy - Std dev Y |
| **F6** | ↓ - Navigate | REG - Regression type |

---

## 🔧 Setup Functions

### Setting Decimal Places
```
SHIFT → . (dot) → Enter digit 0-9 → Display shows "D#"
```
- **0-9**: Fixed decimal places
- **.** (dot): Floating point mode

### Toggle BEGIN/END Mode
```
SHIFT → F4 → Display shows "BGN" or clears
```
- **BGN**: Payments at beginning of period (annuity due)
- **END**: Payments at end of period (ordinary annuity)

### Clear Functions
| Keys | Action |
|------|--------|
| **DEL** | Backspace (delete last digit) |
| **AC** | Clear all TVM values |
| **SHIFT + F5** | CLR TVM (same as AC) |

---

## 📊 Features

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

---

## 🎮 Quick Start

### Key Mappings
| Casio Key | TI Function |
|-----------|-------------|
| **SHIFT** | 2ND mode toggle |
| **SHIFT + F3** | AMORT worksheet |
| **SHIFT + F4** | BGN/END toggle |
| **SHIFT + 7** | Cash Flow |
| **SHIFT + 8** | Bond |
| **SHIFT + 9** | Depreciation |
| **SHIFT + 4** | Statistics |
| **SHIFT + 5** | Date |
| **SHIFT + 6** | Breakeven (Pro only) |
| **SHIFT + 3** | Profit Margin (Pro only) |
| **UP/DOWN** | Navigate worksheet |
| **EXIT** | Return to TVM |
| **OPTN** | STO (store to memory) |
| **VARS** | RCL (recall from memory) |

### Example: Calculate Loan Payment
```
1. Enter: 360 → F1 (N)      ← 30 years × 12 months
2. Enter: 6 → F2 (I/Y)      ← 6% annual rate
3. Enter: 250000 → F3 (PV)  ← Loan amount
4. Enter: 0 → F5 (FV)       ← Pay off completely
5. Press: F6 (CPT) → F4     ← Compute PMT
6. Result: -1,498.88        ← Monthly payment
```

---

## 🔨 Building

### fxSDK (Recommended)
```bash
# Install fxSDK
brew tap Lephenixnoir/fxsdk
brew install fxsdk

# Build
fxsdk build-fx

# Run tests
make cfa-test
```

### Official Casio SDK (Windows)
```bash
# Add preprocessor define: USE_CASIO_SDK
# See docs/CASIO_SDK_BUILD.md for details
```

### Install on Calculator
1. Connect fx-9750GIII via USB
2. Copy `Open-fx-BA.g3a` to calculator
3. Open from Main Menu

---

## ✅ Test Coverage

**39/39 tests passing** including:
- 10 CFA Level I-III exam-style questions
- 10 Advanced financial calculations  
- 5 Worksheet integration tests
- 5 Edge case boundary tests
- 9 Additional worksheet tests

---

## 📁 Project Structure
```
src/
├── main.c           # Entry point & event loop
├── tvm.c/h          # TVM solver & amortization
├── cashflow.c/h     # NPV, IRR, NFV, MIRR
├── bond.c/h         # Bond pricing & duration
├── depreciation.c/h # 6 depreciation methods
├── statistics.c/h   # Stats & regression
├── date.c/h         # Date calculations
├── profit.c/h       # Breakeven & margin
├── display.c/h      # TI-style display
├── hal/             # Hardware abstraction (dual SDK)
│   ├── fxsdk/       # fxSDK implementation
│   └── casio/       # Casio SDK implementation
└── tests.c/h        # CFA validation suite
```

---

## 📄 License

MIT License - See [LICENSE](LICENSE) for details.

---

<div align="center">

**Made with ❤️ for CFA & Finance Students**

</div>
