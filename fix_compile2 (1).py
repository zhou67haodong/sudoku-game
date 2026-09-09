# -*- coding: utf-8 -*-
"""全面扫描 cpp 中的所有方法定义，补全 h 中的缺失声明"""
import re, os

CPP = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuBoard.cpp"
H   = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\SudokuBoard.h"

with open(CPP, encoding='utf-8') as f:
    cpp_text = f.read()

with open(H, encoding='utf-8') as f:
    h_text = f.read()

# 匹配所有 cpp 中的 SudokuBoard:: 方法定义
# 匹配: 返回类型 类名::方法名(参数)
pattern = re.compile(
    r'\b(void|bool|int|double|QPair<\s*int\s*,\s*int\s*>|QSize|QVector<\s*QVector<\s*int\s*>\s*>)'
    r'\s+SudokuBoard::([a-zA-Z_][a-zA-Z0-9_]*)\s*\(',
    re.MULTILINE
)

cpp_methods = {}
for m in pattern.finditer(cpp_text):
    ret_type = m.group(1).strip()
    name = m.group(2)
    # 从定义位置往后读，读到匹配的 { 为止
    start = m.start()
    end = cpp_text.find('{', start)
    if end == -1:
        continue
    # 取参数列表
    paren_start = cpp_text.find('(', start)
    paren_end = cpp_text.rfind(')', paren_start, end)
    params = cpp_text[paren_start:paren_end+1].strip()
    sig = f"{ret_type} {name}{params}"
    cpp_methods[name] = sig

print(f"CPP 中找到 {len(cpp_methods)} 个方法定义:")
for name in sorted(cpp_methods):
    print(f"  {name}")

# 匹配 h 中已有的声明
h_pattern = re.compile(
    r'\b(void|bool|int|double|QPair<\s*int\s*,\s*int\s*>|QVector<\s*QVector<\s*int\s*>\s*>)'
    r'\s+SudokuBoard::([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*(const)?\s*(override)?\s*;',
    re.MULTILINE
)
h_methods = set()
for m in h_pattern.finditer(h_text):
    h_methods.add(m.group(2))

print(f"\nH 中已有 {len(h_methods)} 个方法声明")

# 缺失的
missing = {name: sig for name, sig in cpp_methods.items() if name not in h_methods}
print(f"\n缺失声明 ({len(missing)} 个):")
for name, sig in sorted(missing.items()):
    print(f"  {name}: {sig}")

# 生成补全文本
new_lines = []
for name, sig in sorted(missing.items()):
    # 提取返回类型和参数（处理多行）
    sig_clean = sig.replace('\n', ' ').replace('\s+', ' ')
    m = re.match(r'(.+?)\s+SudokuBoard::([^\(]+)\((.+)\)', sig_clean)
    if m:
        ret_type = m.group(1).strip()
        params_str = m.group(3).strip()
        new_lines.append(f"    {ret_type} {name}({params_str});")
    else:
        print(f"  [WARN] 无法解析: {sig[:80]}")

insert_text = "\n" + "\n".join(new_lines) + "\n"

# 找到 private: 区块末尾（在 #endif 之前）
# 在最后一个 }; 之前插入
ENDMARK = "#endif // SUDOKUBOARD_H"
if ENDMARK in h_text:
    h_text = h_text.replace(ENDMARK, insert_text + ENDMARK)
    print(f"\n已插入 {len(new_lines)} 个缺失声明到 SudokuBoard.h")
else:
    print("ERROR: 找不到 #endif")

with open(H, 'w', encoding='utf-8') as f:
    f.write(h_text)

print("\nDone!")
