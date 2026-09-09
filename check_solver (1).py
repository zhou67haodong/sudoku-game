# -*- coding: utf-8 -*-
hp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\SudokuSolver.h"
cp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuSolver.cpp"
with open(hp, encoding="utf-8") as f:
    h = f.read()
with open(cp, encoding="utf-8") as f:
    c = f.read()
# Find getHint lines
import re
h_match = re.search(r'getHint\([^)]+\)', h)
c_match = re.search(r'getHint\([^)]+\)', c)
print("Header:", h_match.group() if h_match else "NOT FOUND")
print("Cpp:   ", c_match.group() if c_match else "NOT FOUND")
print()
# Show full cpp signature
lines = c.split('\n')
for i, l in enumerate(lines):
    if 'getHint' in l:
        print(f"cpp {i+1}: {l.rstrip()}")
        # Also show next line
        if i+1 < len(lines):
            print(f"cpp {i+2}: {lines[i+1].rstrip()}")
