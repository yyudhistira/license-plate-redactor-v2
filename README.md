# License Plate Redactor

A high-performance C++ application for detecting and redacting license plates in video files using YOLOv8 and OpenCV.

## Features

- **Batch Video Processing**: Process MP4 video files with license plate detection and redaction
- **Memory Efficient**: Handles large 4K videos up to 10GB on systems with 16GB RAM
- **Temporal Consistency**: Smooth, glitch-free redaction across frames using object tracking
- **Multi-threaded Pipeline**: Optimized processing with parallel detection and redaction
- **GPU Acceleration**: Optional GPU support with CPU fallback
- **Cross-platform**: Supports macOS and Linux

## Requirements

- **OS**: macOS or Linux
- **RAM**: 16GB minimum
- **Build Tools**:
  - CMake 3.20 or higher
  - C++20 compatible compiler (GCC 10+, Clang 12+, or Apple Clang 13+)
  - vcpkg package manager
- **Dependencies** (managed by vcpkg):
  - OpenCV 4.8+ with contrib and ffmpeg
  - spdlog
  - CLI11

## Building from Source

### 1. Install vcpkg

If you don't have vcpkg installed:

```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS

# Set environment variable (add to your shell profile)
export VCPKG_ROOT=/path/to/vcpkg
```

### 2. Clone the Repository

```bash
git clone https://github.com/yourusername/license-plate-redactor-v2.git
cd license-plate-redactor-v2
```

### 3. Build the Project

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake (vcpkg will automatically install dependencies)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)  # Linux
cmake --build . -j$(sysctl -n hw.ncpu)  # macOS
```

The build process will automatically download and compile all dependencies through vcpkg. This may take some time on the first build.

### 4. Download Models

Download the YOLOv8 ONNX models:

```bash
# From the project root
cd models
./download-models.sh
```

Or manually download from:
- YOLOv8 vehicle detection: [Download link TBD]
- YOLOv8 license plate detection: [Download link TBD]

Place the `.onnx` files in the `models/` directory.

## Usage

### Basic Usage

```bash
./build/src/cli/license-plate-redactor -i input.mp4 -o output.mp4
```

### Advanced Options

```bash
./build/src/cli/license-plate-redactor \
  -i input.mp4 \
  -o output.mp4 \
  --blur-strength 25 \
  --detection-threshold 0.5 \
  --use-gpu \
  --verbose \
  --threads 4
```

### Command-Line Options

- `-i, --input FILE`: Input video file (required)
- `-o, --output FILE`: Output video file (required)
- `-m, --model PATH`: Path to YOLO model (default: models/yolov8.onnx)
- `-b, --blur-strength INT`: Gaussian blur kernel size (default: 25)
- `-t, --threshold FLOAT`: Detection confidence threshold (default: 0.5)
- `-g, --use-gpu`: Enable GPU acceleration if available
- `-v, --verbose`: Verbose logging
- `-p, --progress`: Show progress bar
- `-j, --threads INT`: Number of detection threads (default: 2)
- `-h, --help`: Show help message

### Batch Processing

Process multiple videos:

```bash
for video in videos/*.mp4; do
  ./license-plate-redactor -i "$video" -o "redacted_$(basename $video)"
done
```

## Architecture

The application uses a multi-threaded pipeline architecture:

1. **Video Reader Thread**: Reads and decodes video frames
2. **Detection Thread Pool**: Detects vehicles and license plates using YOLOv8
3. **Tracker**: Maintains temporal consistency across frames (SORT algorithm)
4. **Redaction Thread**: Applies Gaussian blur to detected regions
5. **Video Writer Thread**: Encodes and writes output video

For more details, see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Performance

Typical processing speeds (on M1 Mac with 16GB RAM):

- **1080p (HD)**: ~1-2x real-time (CPU only)
- **4K (UHD)**: ~0.3-0.5x real-time (CPU only)
- **With GPU**: 5-10x faster

Memory usage remains under 4GB even for large 4K videos.

## Testing

Run unit tests:

```bash
cd build
ctest --output-on-failure
```

Or run specific test suites:

```bash
./tests/unit/core_tests
./tests/integration/pipeline_tests
```

## Documentation

- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Build Instructions](docs/BUILD.md)
- [Usage Guide](docs/USAGE.md)

## Troubleshooting

### Build Issues

**vcpkg dependencies fail to install:**
- Ensure `VCPKG_ROOT` environment variable is set
- Try: `vcpkg integrate install`

**OpenCV not found:**
- Manually install: `vcpkg install opencv4[contrib,ffmpeg]`

### Runtime Issues

**Model not found:**
- Ensure model files are in `models/` directory
- Check file paths with `--verbose` flag

**Out of memory:**
- Reduce detection threads with `--threads 1`
- Process lower resolution videos

**Slow processing:**
- Enable GPU acceleration with `--use-gpu`
- Reduce blur strength for faster processing

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

- [YOLOv8](https://github.com/ultralytics/ultralytics) for object detection
- [OpenCV](https://opencv.org/) for video processing
- [SORT](https://github.com/abewley/sort) for object tracking

## Contact

For questions or issues, please open an issue on GitHub.
