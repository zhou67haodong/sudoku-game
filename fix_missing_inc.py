# -*- coding: utf-8 -*-
"""Fix missing includes in GameState.cpp and ThemeManager.cpp"""
import os

# GameState.cpp - QJsonArray incomplete type
gp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\GameState.cpp"
with open(gp, encoding="utf-8") as f:
    c = f.read()
if "#include <QJsonArray>" not in c:
    c = c.replace("#include <QJsonDocument>", "#include <QJsonDocument>\n#include <QJsonArray>")
    # Or if QJsonDocument not present either, add it
    if "#include <QJsonArray>" not in c:
        c = "#include <QJsonArray>\n" + c
    print("Added QJsonArray include to GameState.cpp")
with open(gp, "w", encoding="utf-8") as f:
    f.write(c)

# ThemeManager.cpp - QStyle incomplete type
tp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\ThemeManager.cpp"
with open(tp, encoding="utf-8") as f:
    c = f.read()
if "#include <QStyle>" not in c and "#include <QCommonStyle>" not in c:
    # QApplication needs QStyle, add it
    c = c.replace("#include <QApplication>", "#include <QApplication>\n#include <QStyle>")
    print("Added QStyle include to ThemeManager.cpp")
with open(tp, "w", encoding="utf-8") as f:
    f.write(c)

print("Done!")
