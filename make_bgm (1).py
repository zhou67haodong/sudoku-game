# -*- coding: utf-8 -*-
"""合成数独游戏背景音乐 bgm.wav（C大调柔和琶音循环，约16秒无缝循环）"""
import math, struct, wave

SR = 44100
DUR = 16.0
N = int(SR * DUR)

# 和弦进行: C - G/B - Am - F  每个和弦4拍(4秒)
CHORDS = [
    (261.63, 329.63, 392.00, 523.25),   # C:  C4 E4 G4 C5
    (246.94, 293.66, 392.00, 493.88),   # G/B: B3 D4 G4 B4
    (220.00, 261.63, 329.63, 440.00),   # Am: A3 C4 E4 A4
    (174.61, 220.00, 261.63, 349.23),   # F:  F3 A3 C4 F4
]

buf = [0.0] * N

def tone(freq, t):
    """柔和音色: 基频 + 轻泛音, 正弦"""
    return (math.sin(2 * math.pi * freq * t)
            + 0.35 * math.sin(2 * math.pi * freq * 2 * t)
            + 0.12 * math.sin(2 * math.pi * freq * 3 * t))

chord_len = DUR / len(CHORDS)
for ci, chord in enumerate(CHORDS):
    start = int(ci * chord_len * SR)
    end = int((ci + 1) * chord_len * SR)
    seg = end - start
    # 琶音模式: 依次弹响和弦各音, 每音1拍, 循环
    note_dur = chord_len / 4.0
    for i in range(seg):
        t_global = (start + i) / SR
        t_in_chord = i / SR
        beat = int(t_in_chord / note_dur)
        freq = chord[beat % 4]
        t_local = t_in_chord - beat * note_dur
        # 音符包络: 快速起音 + 指数衰减
        env = math.exp(-3.0 * t_local / note_dur * 4) * (1 - math.exp(-60 * t_local))
        # 和弦底部持续低音(最弱)
        bass = 0.15 * tone(chord[0] / 2, t_global)
        v = 0.30 * tone(freq, t_global) * env + bass
        # 整体淡入淡出(首尾1秒)实现无缝循环感
        fade = 1.0
        if t_global < 1.0:
            fade = t_global / 1.0
        elif t_global > DUR - 1.0:
            fade = (DUR - t_global) / 1.0
        buf[start + i] += v * fade

# 归一化 + 写文件
peak = max(abs(x) for x in buf)
scale = 0.72 / peak
with wave.open(r"C:/Users/26623/Desktop/Sudoku_Project/SudokuGame/resources/sounds/bgm.wav", "w") as w:
    w.setnchannels(2)
    w.setsampwidth(2)
    w.setframerate(SR)
    frames = bytearray()
    for s in buf:
        v = int(max(-1.0, min(1.0, s * scale)) * 32767)
        frames += struct.pack("<hh", v, v)
    w.writeframes(bytes(frames))

print("bgm.wav OK, %.1fs" % DUR)
