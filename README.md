# Open fx-BA: TI BA II Plus Clone for Casio fx-9750GIII

An open source add in for Casio fx-9750GIII / fx-9860GIII / fx-9860G series, implementing TI BA II Plus (Standard & Professional) financial calculation functions.

## Features

- **TVM (Time Value of Money)**: N, I/Y, PV, PMT, FV
- **Cash Flow Analysis**: NPV, IRR, NFV*, PB*, DPB*
- **Depreciation**: SL, SYD, DB (6 methods)
- **Bond Calculations**: Price, Yield, Duration*
- **Date Calculations**: Days between dates, 30/360 and Actual conventions

*Professional mode only

## Building

### Prerequisites

Install fxSDK on your system:
```bash
# macOS (via Homebrew)
brew tap Lephenixnoir/fxsdk
brew install fxsdk

# Or follow: https://gitea.planet-casio.com/Lephenixnoir/fxsdk
```

### Compile

```bash
fxsdk build-fx
```

### Install on Calculator

1. Connect your fx-9750GIII via USB
2. Copy `Open-fx-BA.g3a` to the calculator
3. Open from Main Menu

## Project Structure

```
src/
├── main.c         # Entry point & event loop
├── types.h        # Data structures
├── config.h       # Constants & feature flags
├── ui.c/h         # Screen rendering
├── input.c/h      # Input handling & state machine
├── tvm.c/h        # TVM solver
└── cashflow.c/h   # Cash flow analysis
```

## License

MIT License
