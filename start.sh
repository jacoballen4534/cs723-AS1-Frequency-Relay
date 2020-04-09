#!/bin/bash
PATH_TO_NIOS_SETUP="C:/altera/13.0/nios2eds"

PROJECT_PATH=$(pwd)

if [ $1 ] && [ $1 == "simulator" ]; then
    if [ $2 ] && [ $2 == "clean" ]; then
        cd Simulations/gp21_cs723_a1/Build
        make clean
    elif [ -z $2 ] || ([ $2 ] &&  [ $2 == "all" ]); then
        cd Simulations/gp21_cs723_a1/Build
        make all
        cd ../../..
        start "${PROJECT_PATH}/Simulations/gp21_cs723_a1/Build/gp21_cs723_a1_simulator.exe"
    fi
elif [ $1 ] && [ $1 == "nios" ]; then
    echo "Moving to nios2 folder"
    cd $PATH_TO_NIOS_SETUP

    if [ $2 ] && [ $2 == "clean" ]; then
        echo "Running Nios II shell start script"
        ./nios2_command_shell.sh bash -c "cd $PROJECT_PATH/Nios/software/gp21_cs723_a1; make clean_all;"
    elif [ -z $2 ] || ([ $2 ] &&  [ $2 == "all" ]); then
        echo "Running Nios II shell start script"
        ./nios2_command_shell.sh bash -c "cd $PROJECT_PATH/Nios/software/gp21_cs723_a1; make;"
    fi
fi
    