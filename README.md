# Senior_Project_2025

Assembly Analyzer, v1.0.0

Authors: Kaiden Robinson, Hannah Hall, Brian Boggs

This project is an application that will parse ARM assembly code, gather metrics, search items by line, and reveal potential errors in the code.
It is designed to be used on Raspberry Pi, available in both UNIX Command Line and GUI versions.

Requirements:
  Raspberry Pi Zero, Raspberry Pi 3, or Raspberry Pi 4
  Raspberry Pi must have GNU Compiler Collection (GCC) 6.2017-q2 or later pre-installed.
  Permission to install Qt5 is required to install the GUI version.

For UNIX Command Line Version:

  Compile and Run instructions:
  1) To build the project:
    Open the terminal of Raspberry Pi and navigate (cd) to the project folder "../Senior_Project_2025/SeniorProject2025".
    Then run the following command:

      g++ -std=c++17 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp constantsLabelsAndDataElements.cpp pushPopErrors.cpp Error.cpp registerAndStringErrors.cpp 

  2) The default executable name is "main". Once compiled, you can start the program with the following command at the same folder location:
    
      ./main

    At this point, you may input a .s file to the project to be checked.
    As an example, see the following command and replace "filename.s" with your .s file path
    (We recommend you right click on the .s file, click 'Copy Path', and paste it to the terminal to do this.
    If a folder in the file path has a space, use quotations around the file path in the command):

      ./main -f "filename.s"

  Command Line Help:
    For more help related to project commands, view the following:
    
      Usage: ./main [options]
      Options:
        -h                  Show this help message
        -f <file>           Input ARM .s file (use quotations if spaces in file name)
        -d <directory>      Input directory of .s files (use quotations if spaces in directory name)
        --noff              Disables fast-forward when reading from a directory, pausing between each file.
        --csv               Output selected data to CSV file
        --metrics           Show and optionally save summary metrics
        --lines             Show and optionally save line-by-line data
      Examples:
        ./main -f test.s --metrics --csv       Output only metrics to CSV and console
        ./main -f test.s --lines --csv         Output only line-by-line data to CSV and console
        ./main -f test.s --metrics --lines --csv  Output both types to CSV and console

  CSV datafiles are output to the "output" folder in the "SeniorProject2025" folder.

For GUI version:

  Qt5 must be installed on the Raspberry Pi. You may use the following command to install it (requires permissions):
  
    sudo apt install qtbase5-dev

  Compile and Run Instructions:
    1) Build Executable:
      On the terminal, navigate (cd) to the "SeniorProject2025" folder and run the following commands:

        qmake
        make

    2) The default name of the GUI is "SeniorProject2025". To run the GUI:
      In the same directory run the following command to start the GUI:

        ./SeniorProject2025

      Alternatively, you may double click the executable file.

  GUI Help:
    Input a .s file to start
    Within the main window, you may click 'File' -> 'Open' and input a .s file to open as a new tab.
    Potential errors discovered by the project will be highlighted and output to 'Console' after you press the 'Check' button.
    
