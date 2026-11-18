# THERMO pour Casio Graph 35+E II / fx-9750GIII (fxSDK + gint).  Compilation :  fxsdk build-fx
cmake_minimum_required(VERSION 3.15)
project(Thermo)

include(GenerateG1A)
find_package(Gint 2.1 REQUIRED)

add_executable(thermo src/thermo.c)
target_compile_options(thermo PRIVATE -Wall -Wextra -Os)
target_link_libraries(thermo Gint::Gint)

generate_g1a(TARGET thermo OUTPUT "Thermo.g1a" NAME "Thermo" ICON assets-fx/icon.png)
