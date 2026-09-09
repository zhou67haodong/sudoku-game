# -*- coding: utf-8 -*-
import os, re
src_dir = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame"
include_dir = os.path.join(src_dir, "include")
headers = sorted([f for f in os.listdir(include_dir) if f.endswith(".h")])
print("Headers:", headers)
for h in headers:
    path = os.path.join(include_dir, h)
    with open(path, encoding="utf-8") as f:
        content = f.read()
    incs = re.findall(r'#include\s+"([^"]+)"', content)
    for inc in incs:
        if inc not in headers:
            print(f"  WARN: {h} -> {inc} (MISSING)")
print("Check done")
