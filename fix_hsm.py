# -*- coding: utf-8 -*-
"""修复所有文件中 HighScoreManager -> HighScore 的引用"""
import os, re

src = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src"
inc = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include"
out = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\fix_hsm_report.txt"

report = []
def fix_file(path):
    with open(path, encoding="utf-8") as f:
        c = f.read()
    orig = c
    # Replace HighScoreManager -> HighScore everywhere
    c = c.replace("HighScoreManager", "HighScore")
    # Remove duplicate #include "HighScore.h" that might appear
    # Count changes
    changes = orig.count("HighScoreManager")
    if changes > 0:
        with open(path, "w", encoding="utf-8") as f:
            f.write(c)
        report.append(f"  Fixed {changes}x in {os.path.basename(path)}")
    return changes

total = 0
for d in [src, inc]:
    for fname in os.listdir(d):
        if fname.endswith(".cpp") or fname.endswith(".h"):
            p = os.path.join(d, fname)
            n = fix_file(p)
            total += n

report.append(f"\nTotal: {total} replacements")
print("\n".join(report))
with open(out, "w", encoding="utf-8") as f:
    f.write("\n".join(report))
print("Done!")
