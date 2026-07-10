import wave
import struct
from pathlib import Path

wav_path = Path(r"C:\Zed_Projects\zed_audio_clip_player\audio_files\Boom_Boom_5sec_12k_mono.wav")
mem_path = Path(r"C:\Zed_Projects\zed_audio_clip_player\audio_files\audio_samples.mem")

EXPECTED_SAMPLE_RATE = 12000
EXPECTED_CHANNELS = 1
EXPECTED_SAMPLE_WIDTH_BYTES = 2
EXPECTED_SECONDS = 5
EXPECTED_SAMPLES = EXPECTED_SAMPLE_RATE * EXPECTED_SECONDS

if not wav_path.exists():
    raise FileNotFoundError(f"WAV file not found: {wav_path}")

with wave.open(str(wav_path), "rb") as wav:
    channels = wav.getnchannels()
    sample_width = wav.getsampwidth()
    sample_rate = wav.getframerate()
    total_frames = wav.getnframes()

    print("Input WAV:")
    print(f"  File: {wav_path}")
    print(f"  Channels: {channels}")
    print(f"  Sample width: {sample_width} bytes")
    print(f"  Sample rate: {sample_rate} Hz")
    print(f"  Total frames: {total_frames}")

    if channels != EXPECTED_CHANNELS:
        raise ValueError("Expected mono WAV file")

    if sample_width != EXPECTED_SAMPLE_WIDTH_BYTES:
        raise ValueError("Expected 16-bit PCM WAV file")

    if sample_rate != EXPECTED_SAMPLE_RATE:
        raise ValueError("Expected 12 kHz WAV file")

    frames_to_read = min(total_frames, EXPECTED_SAMPLES)
    raw_data = wav.readframes(frames_to_read)
    samples = list(struct.unpack("<" + "h" * frames_to_read, raw_data))

if len(samples) < EXPECTED_SAMPLES:
    samples.extend([0] * (EXPECTED_SAMPLES - len(samples)))

if len(samples) > EXPECTED_SAMPLES:
    samples = samples[:EXPECTED_SAMPLES]

with open(mem_path, "w") as mem:
    for sample in samples:
        mem.write(f"{sample & 0xFFFF:04X}\n")

print()
print("Output MEM:")
print(f"  File: {mem_path}")
print(f"  Samples written: {len(samples)}")
print(f"  Duration: {len(samples) / EXPECTED_SAMPLE_RATE:.2f} seconds")
print()
print("Conversion complete.")