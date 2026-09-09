# -*- coding: utf-8 -*-
p = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\MainWindow.cpp"
with open(p, encoding="utf-8") as f:
    c = f.read()
print("HighScoreManager" in c)
c = c.replace('#include "HighScoreManager.h"', '#include "HighScore.h"')
print("HighScoreManager" in c)
with open(p, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed MainWindow.cpp")
