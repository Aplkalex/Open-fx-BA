# Casio SDK Build Guide (Windows)

## Prerequisites
- Windows PC
- Casio fx-9860G SDK or FA-124
- Your Open fx-BA source code

---

## Step 1: Create New SDK Project

1. Open **fx-9860G SDK**
2. File → New → Add-in Project
3. Name: `OpenFxBA`
4. Location: Choose a folder

---

## Step 2: Copy Source Files

Copy these folders to your SDK project:

```
Open fx-BA/src/  →  SDK_Project/src/
```

**Files to copy:**
```
src/
├── hal/           (entire folder)
├── tvm.c, tvm.h
├── bond.c, bond.h
├── cashflow.c, cashflow.h
├── depreciation.c, depreciation.h
├── statistics.c, statistics.h
├── date.c, date.h
├── profit.c, profit.h
├── memory.c, memory.h
├── input.c, input.h
├── display.c, display.h
├── keyboard.c, keyboard.h
├── screens.c, screens.h
├── worksheets.c, worksheets.h
├── features.c, features.h
├── fonts.c, fonts.h
├── ui.c, ui.h
├── types.h
├── config.h
└── main.c
```

---

## Step 3: Update Project Settings

In SDK IDE:
1. Project → Properties → C/C++ 
2. Add preprocessor define:
   ```
   USE_CASIO_SDK
   ```

Or edit project file to add:
```
CFLAGS += -DUSE_CASIO_SDK
```

---

## Step 4: Create main.c Entry Point

Casio SDK需要特定的 entry point。修改 main.c 開頭：

```c
#ifdef USE_CASIO_SDK
#include <fxlib.h>

int AddIn_main(int isAppli, unsigned short OptionNum) {
    // Your calculator code here
    calculator_main();
    return 1;
}

#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

#pragma section _TOP
int InitializeSystem(int isAppli, unsigned short OptionNum) {
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}
#pragma section
#endif
```

---

## Step 5: Build

1. Build → Build Project (F7)
2. Check for errors
3. Output: `OpenFxBA.g3a` or `OpenFxBA.g1a`

---

## Step 6: Transfer to Calculator

### Method A: USB + FA-124
1. Connect calculator via USB
2. Open FA-124
3. Drag .g3a file to calculator
4. Disconnect

### Method B: SD Card (fx-9750GIII)
1. Copy .g3a to SD card root
2. Insert SD into calculator
3. LINK → Receive → SD Card

---

## Step 7: Run

1. On calculator: MENU
2. Find "OpenFxBA" icon
3. Press EXE to launch

---

## Troubleshooting

### Error: "fxlib.h not found"
→ Check SDK include paths

### Error: "undefined reference to..."
→ Add missing .c files to project

### Display issues
→ Check `hal/casio/hal_display_casio.c` implementation

### Other issues
→ Leave a issue on GitHub: https://github.com/aplkalex/Open-fx-BA/issues