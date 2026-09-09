# -*- coding: utf-8 -*-
"""Fix GameState history JSON issue"""
gp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\GameState.cpp"
with open(gp, encoding="utf-8") as f:
    c = f.read()

# The proper fix: convert QVector<QJsonObject> to QJsonArray
old = '    root["history"] = m_history;'
new = '    {\n        QJsonArray histArr;\n        for (const QJsonObject &item : m_history) histArr.append(item);\n        root["history"] = histArr;\n    }'
if old in c:
    c = c.replace(old, new)
    print("Fixed history assignment")
else:
    print("Not found, searching...")
    idx = c.find("root[\"history\"]")
    print(repr(c[max(0,idx-20):idx+60]))
with open(gp, "w", encoding="utf-8") as f:
    f.write(c)
