# -*- coding: utf-8 -*-
with open(r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\MainWindow.h", encoding='utf-8') as f:
    c = f.read()
c = c.replace('#include "HighScoreManager.h"', '#include "HighScore.h"')
print('SoundEffect in c:', 'SoundEffect' in c)
print('QSoundEffect in c:', 'QSoundEffect' in c)
print('SoundManager in c:', 'SoundManager' in c)
with open(r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\include\MainWindow.h", 'w', encoding='utf-8') as f:
    f.write(c)
print('Fixed!')
