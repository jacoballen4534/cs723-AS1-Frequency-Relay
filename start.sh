#!/bin/bash
PATH_TO_NIOS_SETUP=$SOPC_KIT_NIOS2
PROJECT_PATH=$(pwd -W)

if [ $1 ] && [ $1 == "simulator" ]; then
    if [ $2 ] && [ $2 == "clean" ]; then
        cd Simulations/gp21_cs723_a1/Build
        make clean
    elif [ $2 ] && [ $2 == "app" ]; then
        cd Simulations/gp21_cs723_a1/Build
        make all
        ./gp21_cs723_a1_simulator.exe | py ../../../App/display_app.py
    elif [ -z $2 ] || ([ $2 ] &&  [ $2 == "all" ]); then
        cd Simulations/gp21_cs723_a1/Build
        make all
        start "gp21_cs723_a1_simulator.exe"
    fi
elif [ $1 ] && [ $1 == "nios" ]; then
    echo "Moving to nios2 folder"
    cd $PATH_TO_NIOS_SETUP

    if [ $2 ] && [ $2 == "clean" ]; then
        echo "Running Nios II shell start script"
        ./nios2_command_shell.sh bash -c "cd $PROJECT_PATH/Nios/software/gp21_cs723_a1; make clean_all;"
    elif [ -z $2 ] || ([ $2 ] &&  [ $2 == "all" ]); then
        echo "Running Nios II shell start script"
        ./nios2_command_shell.sh bash -c "cd $PROJECT_PATH/Nios/software/gp21_cs723_a1; make"
        echo "cd $PROJECT_PATH/App; nios2-terminal | py display_app.py & nios2-download -g ../Nios/software/gp21_cs723_a1/gp21_cs723_a1.elf" | ./'Nios II Command Shell.bat'
    fi
fi
    