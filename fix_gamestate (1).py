# -*- coding: utf-8 -*-
"""Fix GameState history JSON issue and create placeholder wav files"""
import struct, wave, os, math

# === Fix GameState.cpp history assignment ===
gp = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\src\GameState.cpp"
with open(gp, encoding="utf-8") as f:
    c = f.read()

# Fix: root["history"] = m_history; -> convert to QJsonArray first
# Find and replace
old = '    root["history"] = m_history;'
new = '    root["history"] = QJsonArray::fromVariantList(QVariantList::fromList(\n        QList<QVariant>::fromStdList(\n            std::list<QVariant>(m_history.begin(), m_history.end()))));'
# Simpler approach - just loop
c = c.replace(
    '    root["history"] = m_history;',
    '    {\n        QJsonArray histArr;\n        for (const QJsonObject &obj : m_history) histArr.append(obj);\n        root["history"] = histArr;\n    }'
)
with open(gp, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed GameState.cpp history assignment")

# === Create placeholder wav files ===
def create_wav(path, duration_sec=0.3, freq=440):
    """Create a simple sine wave wav file"""
    import wave, struct, math, array
    sample_rate = 22050
    n_samples = int(sample_rate * duration_sec)
    # Generate sine wave samples
    samples = []
    for i in range(n_samples):
        t = i / sample_rate
        v = int(32767 * 0.3 * math.sin(2 * math.pi * freq * t))
        samples.append(struct.pack('<h', v))
    
    with wave.open(path, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(b''.join(samples))

sounds_dir = r"C:\Users\10103\Desktop\sudoku_project\SudokuGame\resources\sounds"
os.makedirs(sounds_dir, exist_ok=True)

sound_files = {
    "cellselect.wav": (0.1, 800),
    "cellfill.wav": (0.2, 600),
    "error.wav": (0.3, 300),
    "hint.wav": (0.4, 500),
    "victory.wav": (0.8, 440),
}
for fname, (dur, freq) in sound_files.items():
    fpath = os.path.join(sounds_dir, fname)
    if not os.path.exists(fpath):
        create_wav(fpath, dur, freq)
        print(f"Created: {fname}")
    else:
        print(f"Exists: {fname}")

print("All done!")
