# -*- coding: utf-8 -*-
"""直接修复 SudokuGenerator.cpp 和 SudokuSolver.cpp"""
import os

# === SudokuGenerator.cpp ===
gpath = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuGenerator.cpp"
with open(gpath, encoding="utf-8") as f:
    c = f.read()

# 1. Replace qsrand
c = c.replace(
    "qsrand(static_cast<uint>(QTime::currentTime().msecsSinceStartOfDay()));",
    "// Qt6: RNG auto-seeds"
)
# 2. Replace qrand() % N
import re
def qrand_sub(m):
    n = int(m.group(1))
    return f"QRandomGenerator::global()->bounded({n})"
c = re.sub(r"qrand\(\)\s*%\s*(\d+)", qrand_sub, c)

# 3. Add includes
if "#include <algorithm>" not in c:
    c = c.replace("#include <QRandomGenerator>", "#include <QRandomGenerator>\n#include <algorithm>\n#include <vector>")

# 4. Fix shuffle(positions)
old = "    // 随机打乱挖洞顺序\n    shuffle(positions);"
new = """    // 随机打乱挖洞顺序（用 std::shuffle）
    std::vector<std::pair<int,int>> tmp(positions.begin(), positions.end());
    std::shuffle(tmp.begin(), tmp.end(), *QRandomGenerator::global());
    for (int i = 0; i < (int)tmp.size(); ++i) positions[i] = tmp[i];"""
if old in c:
    c = c.replace(old, new)
    print("Fixed shuffle(positions)")
else:
    # Try without \n
    idx = c.find("shuffle(positions)")
    print(f"shuffle(positions) found at {idx}")
    if idx > 0:
        print(repr(c[idx-200:idx+100]))

with open(gpath, "w", encoding="utf-8") as f:
    f.write(c)
print(f"Done fixing {gpath}")
print("qsrand in c:", "qsrand" in c)
print("qrand() in c:", "qrand()" in c)

# === SudokuSolver.cpp ===
spath = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuSolver.cpp"
if os.path.exists(spath):
    with open(spath, encoding="utf-8") as f:
        s = f.read()
    # Fix: solve() called on const object -> make getHint non-const or add const_cast
    # Change getHint signature from const to non-const
    sh_path = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\SudokuSolver.h"
    with open(sh_path, encoding="utf-8") as f:
        sh = f.read()
    # In h: getHint(...) const -> remove const
    if "getHint" in sh:
        sh = re.sub(r"(bool getHint\([^)]+\))\s*const", r"\1", sh)
        print("Fixed getHint const in .h")
        with open(sh_path, "w", encoding="utf-8") as f:
            f.write(sh)
    print(f"Done fixing {spath}")
