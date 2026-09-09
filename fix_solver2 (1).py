# -*- coding: utf-8 -*-
cp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuSolver.cpp"
with open(cp, encoding="utf-8") as f:
    lines = f.readlines()
for i, l in enumerate(lines):
    if 'bool SudokuSolver::getHint' in l:
        print(f"{i+1}: {repr(l)}")
    if 'outNum) const' in l or 'outNum)  const' in l:
        print(f"{i+1}: {repr(l)}")
        lines[i] = l.replace(') const', ')')
        print(f"Fixed at {i+1}")
with open(cp, "w", encoding="utf-8") as f:
    f.writelines(lines)
print("Done")
