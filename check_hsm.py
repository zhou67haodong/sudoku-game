# -*- coding: utf-8 -*-
p = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\MainWindow.cpp"
with open(p, encoding="utf-8") as f:
    lines = f.readlines()
for i, l in enumerate(lines):
    if "HighScore" in l:
        print(f"{i+1}: {repr(l)}")
