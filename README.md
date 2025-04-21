# Senior_Project_2025
This project is an application that will parse ARM assembly code, reveal potential errors in the code, and track data to avoid copying of programs
Authors: Kaiden, Hannah, Brian, Zoe

Usage: ./main [options]
Options:
  -h                  Show this help message
  -f <file>           Input ARM .s file (use quotations if spaces in file name)
  -d <directory>      Input directory of .s files (use quotations if spaces in directory name)
  --csv               Output selected data to CSV file
  --metrics           Show and optionally save summary metrics
  --lines             Show and optionally save line-by-line data
Examples:
  ./main -f test.s --metrics --csv       Output only metrics to CSV and console
  ./main -f test.s --lines --csv         Output only line-by-line data to CSV and console
  ./main -f test.s --metrics --lines --csv  Output both types to CSV and console