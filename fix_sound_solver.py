# -*- coding: utf-8 -*-
"""Fix SoundType enum refs and SudokuSolver signature"""
import os, re

# Fix MainWindow.cpp - replace SoundManager::CellSelect etc -> SoundType::CellSelect
mp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\MainWindow.cpp"
with open(mp, encoding="utf-8") as f:
    c = f.read()
# Replace SoundManager::CellSelect -> SoundType::CellSelect
# etc.
for name in ["CellSelect", "CellFill", "Error", "Hint", "Victory"]:
    c = c.replace(f"SoundManager::{name}", f"SoundType::{name}")
with open(mp, "w", encoding="utf-8") as f:
    f.write(c)
print(f"Fixed SoundManager refs in MainWindow.cpp")

# Fix SudokuSolver - signature mismatch
# .h: bool getHint(const QVector<QVector<int>>& board, int& row, int& col, int& value);
# .cpp: bool getHint(const QVector<QVector<int>>& board, int& row, int& col, int& value) const { ... }
# Fix: remove const from .cpp
sp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuSolver.cpp"
with open(sp, encoding="utf-8") as f:
    c = f.read()
# Fix the signature line
c = re.sub(r"(bool SudokuSolver::getHint\(.*?\))\s+const\s*\{", r"\1 {", c)
# Also check the line - it might be split across lines
# Better: find the getHint definition and remove const before {
# Pattern: bool SudokuSolver::getHint(...\n...) const {
# Let's find and fix it
lines = c.split('\n')
new_lines = []
for i, l in enumerate(lines):
    if 'bool SudokuSolver::getHint' in l:
        # Check if next significant line has const {
        combined = l
        j = i + 1
        while j < len(lines) and lines[j].strip() == '':
            j += 1
        if j < len(lines) and '{' in lines[j]:
            # Remove ' const' before '{'
            brace_line = lines[j]
            brace_line = brace_line.replace(' const {', ' {').replace(' const{', '{')
            new_lines.append(l)
            new_lines.append(brace_line)
            print(f"Fixed SudokuSolver::getHint const at line {i+1}")
            continue
    new_lines.append(l)
c = '\n'.join(new_lines)
with open(sp, "w", encoding="utf-8") as f:
    f.write(c)
print(f"Fixed SudokuSolver.cpp")
print("Done!")
