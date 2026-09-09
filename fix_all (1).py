# -*- coding: utf-8 -*-
"""修复所有 Qt6 编译不兼容问题"""
import re

def fix_qt6_rng(content):
    """替换 qsrand/qrand 为 QRandomGenerator"""
    # qsrand 行
    content = re.sub(
        r'qsrand\(static_cast<uint>\(QTime::currentTime\(\)\.msecsSinceStartOfDay\(\)\)\);',
        '// Qt6: RNG auto-seeds; no explicit srand needed',
        content
    )
    # qrand() % N
    content = re.sub(r'qrand\(\)\s*%(\s*\d+)', 
                     lambda m: f'int(QRandomGenerator::global()->bounded({m.group(1).strip()}))', 
                     content)
    # 确保有 QRandomGenerator 包含
    if '#include <QRandomGenerator>' not in content and 'QRandomGenerator::global()' in content:
        if '#include <QTime>' in content:
            content = content.replace('#include <QTime>', '#include <QTime>\n#include <QRandomGenerator>')
    return content

def fix_generator_cpp():
    path = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SudokuGenerator.cpp"
    with open(path, encoding='utf-8') as f:
        c = f.read()
    
    # 1. 替换 qsrand/qrand
    c = re.sub(
        r'qsrand\(static_cast<uint>\(QTime::currentTime\(\)\.msecsSinceStartOfDay\(\)\)\);',
        '// Qt6 auto-seeds RNG, no explicit srand needed',
        c
    )
    c = re.sub(r'qrand\(\)\s*%(\s*\d+)', 
               lambda m: f'int(QRandomGenerator::global()->bounded({int(m.group(1).strip())}))', c)
    
    # 2. 添加 QRandomGenerator 包含
    if '#include <QRandomGenerator>' not in c:
        c = c.replace('#include <QTime>', '#include <QTime>\n#include <QRandomGenerator>')
    
    # 3. 修复 shuffle(positions) - positions 是 QPair，不能用 shuffle(int&)
    # 改用 std::shuffle
    c = c.replace(
        '    // 随机打乱挖洞顺序\n    shuffle(positions);',
        '    // 随机打乱挖洞顺序（用 std::shuffle）\n'
        '    std::vector<QPair<int,int>> v(positions.begin(), positions.end());\n'
        '    std::shuffle(v.begin(), v.end(), *QRandomGenerator::global());\n'
        '    for (int i = 0; i < v.size(); ++i) positions[i] = v[i];'
    )
    # 移除旧的 shuffle(QVector<int>&) 对 positions 的任何其他调用
    # positions 是 QList<QPair<int,int>>，用 std::shuffle
    
    with open(path, 'w', encoding='utf-8') as f:
        f.write(c)
    print(f"Fixed: {path}")

def fix_soundmanager():
    """检查 SoundManager 是否有 qsrand/qrand"""
    path = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\SoundManager.cpp"
    if not os.path.exists(path):
        print(f"Skipped (not found): {path}")
        return
    with open(path, encoding='utf-8') as f:
        c = f.read()
    if 'qrand' in c or 'qsrand' in c:
        c = fix_qt6_rng(c)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(c)
        print(f"Fixed: {path}")
    else:
        print(f"OK (no qrand): {path}")

import os
fix_generator_cpp()
fix_soundmanager()
print("All Qt6 fixes applied!")
