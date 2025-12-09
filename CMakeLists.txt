cmake_minimum_required(VERSION 3.15)
project(OpenFxBA C)

include(GenerateG1A)
include(GenerateG3A)
include(Fxconv)

find_package(Gint 2.11 REQUIRED)

# All source files
set(SOURCES
    src/main.c
    src/tvm.c
    src/bond.c
    src/cashflow.c
    src/depreciation.c
    src/statistics.c
    src/date.c
    src/profit.c
    src/memory.c
    src/input.c
    src/display.c
    src/keyboard.c
    src/screens.c
    src/worksheets.c
    src/features.c
    src/fonts.c
    src/ui.c
    src/tests.c
    src/hal/fxsdk/hal_display_fxsdk.c
    src/hal/fxsdk/hal_keyboard_fxsdk.c
    src/hal/fxsdk/hal_system_fxsdk.c
)

# Assets (icons)
set(ASSETS)
set(ASSETS_fx assets-fx/icon.png)
set(ASSETS_cg)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(myaddin ${SOURCES} ${ASSETS} ${ASSETS_fx} ${ASSETS_cg})
target_compile_options(myaddin PRIVATE -Wall -Wextra -Os -DUSE_FXSDK)
target_include_directories(myaddin PRIVATE src)
target_link_libraries(myaddin Gint::Gint)

generate_g1a(TARGET myaddin OUTPUT "OpenFxBA.g1a" NAME "OpenFxBA" ICON assets-fx/icon.png)
generate_g3a(TARGET myaddin OUTPUT "OpenFxBA.g3a" NAME "OpenFxBA" ICONS assets-fx/icon-uns.png assets-fx/icon-sel.png)
