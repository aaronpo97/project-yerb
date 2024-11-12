#!/bin/bash

cd "$(dirname "$0")"

if ! command -v ffmpeg &>/dev/null; then
    echo "ffmpeg could not be found."
    exit 1
fi

for file in ./assets/audio/samples/*; do
    if [[ "$file" != *.wav ]]; then
        ffmpeg -i "$file" "${file%.*}.wav"
        echo "Converted $file to WAV"
    fi
done

for file in ./assets/audio/tracks/*; do
    if [[ "$file" != *.ogg ]]; then
        ffmpeg -i "$file" "${file%.*}.ogg"
        echo "Converted $file to OGG"
    fi
done

echo "Audio file conversion complete!"
exit 0
