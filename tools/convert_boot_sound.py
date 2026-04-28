#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
import sys
import wave
from pathlib import Path


def run(command):
    completed = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr.strip() or completed.stdout.strip() or f"command failed: {command}")
    return completed


def convert_with_ffmpeg(source: Path, wav_path: Path, sample_rate: int, duration_seconds: float | None):
    ffmpeg = os.environ.get("FFMPEG") or shutil.which("ffmpeg")
    if ffmpeg is None:
        raise RuntimeError("ffmpeg is required to convert audio assets into boot PCM")
    wav_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        ffmpeg,
        "-y",
        "-i",
        str(source),
        "-ac",
        "1",
        "-ar",
        str(sample_rate),
        "-sample_fmt",
        "s16",
    ]
    if duration_seconds is not None:
        command.extend(["-t", str(duration_seconds)])
    command.append(str(wav_path))
    run(command)


def read_pcm(wav_path: Path):
    with wave.open(str(wav_path), "rb") as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 2:
            raise RuntimeError("converted boot audio must be mono 16-bit PCM")
        sample_rate = wav.getframerate()
        frame_count = wav.getnframes()
        data = wav.readframes(frame_count)
    samples = [int.from_bytes(data[index:index + 2], "little", signed=True) for index in range(0, len(data), 2)]
    return sample_rate, samples


def write_source(output: Path, sample_rate: int, samples, symbol_prefix: str):
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", encoding="utf-8", newline="\n") as file:
        file.write("#include <cstddef>\n")
        file.write("#include <cstdint>\n\n")
        file.write("namespace flintos {\n\n")
        file.write(f"extern const uint32_t {symbol_prefix}SampleRate = {sample_rate};\n")
        file.write(f"extern const std::size_t {symbol_prefix}SampleCount = {len(samples)};\n")
        file.write(f"extern const int16_t {symbol_prefix}Pcm[] = {{\n")
        for index in range(0, len(samples), 12):
            chunk = samples[index:index + 12]
            file.write("    ")
            file.write(", ".join(str(sample) for sample in chunk))
            file.write(",\n")
        file.write("};\n\n")
        file.write("} // namespace flintos\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--sample-rate", default=24000, type=int)
    parser.add_argument("--duration", default=None, type=float)
    parser.add_argument("--symbol-prefix", default="bootSound")
    args = parser.parse_args()

    wav_path = args.output.with_suffix(".wav")
    convert_with_ffmpeg(args.input, wav_path, args.sample_rate, args.duration)
    sample_rate, samples = read_pcm(wav_path)
    write_source(args.output, sample_rate, samples, args.symbol_prefix)


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(1)
