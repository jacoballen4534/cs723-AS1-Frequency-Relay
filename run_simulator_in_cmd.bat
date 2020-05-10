@echo off
cd Simulations/gp21_cs723_a1/Build
make all
gp21_cs723_a1_simulator.exe | ..\..\..\App\dist\display_app.exe