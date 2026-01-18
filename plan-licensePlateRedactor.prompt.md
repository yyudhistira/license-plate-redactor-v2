# License Plate Redactor Desktop Application - Plan

## Goal Description
Create a local desktop application that processes MP4 video files, detects cars and license plates, and redacts (blurs/greys out) the license plates.

## Requirements
- **Input**: Video file in MP4 format (future formats planned later)
- **Output**: Another video file in MP4 format (same format as input)
- **Functionality**: Scan video, identify cars, detect license plates, replace license plate images with grey/blurred boxes
- **Performance**: Run efficiently on macOS/Linux with 16GB RAM
- **Video Support**: Handle large HD/4K videos up to 10GB
- **Speed**: Reasonable processing speed
- **Output Size**: At max double the input size
- **Coverage**: Handle all cases including:
  - Unreadable plates (distance, speed, orientation)
  - Front and back license plates
  - Parked cars and turning cars
  - Multiple cars in one view
- **Technology**: C++ with CMake and vcpkg
- **Distribution**: Prepared for GitHub with README
- **Redaction Quality**: Continuous redaction without glitching between frames

## Technology Stack

### Core Libraries
- **OpenCV 4.8+**: Video I/O, frame processing, DNN module
  - Features: VideoCapture, VideoWriter, CUDA support (optional)
  - vcpkg: `opencv[contrib,ffmpeg,world]`
- **FFmpeg** (via OpenCV): Codec support for various formats

### Object Detection
- **YOLOv8**: Object detection for vehicles and license plates
  - **Stage 1 - Vehicle Detection**: Standard object detection to locate vehicles
  - **Stage 2 - License Plate Segmentation**: Segmentation model (car-plate-seg) to precisely identify license plate regions
    - Provides pixel-level segmentation masks instead of bounding boxes
    - Blur only the exact license plate area for maximum unreadability
    - More accurate than box-based detection for irregular plate shapes
- **ONNX Runtime** (optional): Optimized model inference
- **Model Format**: ONNX for cross-platform compatibility

### Build System
- **CMake 3.20+**: Build configuration
- **vcpkg**: C++ dependency management with CMAKE_TOOLCHAIN_FILE integration

### Additional Libraries
- **Threading**: C++20 `std::jthread` or Intel TBB for parallel processing
- **spdlog**: Fast logging for debugging
- **CLI Parsing**: cxxopts or CLI11 for command-line argument parsing

## Architecture

### Memory-Efficient Design Principles

**Memory Budget (16GB RAM)**:
- 4K frame (3840×2160, BGR): ~25 MB uncompressed
- Double buffering (2 frames): ~50 MB
- Model inference workspace: ~500 MB - 2 GB
- System overhead: ~2 GB
- **Available for processing**: ~11-13 GB

**Key Patterns**:
1. **Frame-by-Frame Streaming**: Never load entire video into memory
2. **Memory Pool Pattern**: Pre-allocate fixed-size buffers, reuse cv::Mat objects
3. **Bounded Queues**: Control memory usage with fixed-size queues between pipeline stages
4. **ROI Processing**: Only process detected vehicle regions for license plate detection
5. **Temporal Consistency**: Track detections across frames to prevent glitching

### Processing Pipeline Architecture

**Multi-Threaded Pipeline**:
```
[Video Reader Thread]
        ↓ (bounded queue: 8-16 frames)
[Detection Thread Pool] (2-4 workers)
    • Vehicle detection (YOLO)
    • License plate detection on vehicle ROIs
    • Temporal tracking for consistency
        ↓ (detection metadata)
[Redaction Thread]
    • Apply Gaussian blur to detected regions
    • Smooth transitions between frames
        ↓ (redacted frames)
[Video Writer Thread]
    • Encode and write to output file
```

**Pipeline Stages**:

1. **Video Reader Thread**
   - Read compressed frames from disk
   - Decode using hardware acceleration (VideoToolbox on macOS)
   - Push to detection queue

2. **Detection Thread Pool** (2-4 threads)
   - Stage 1: Vehicle detection using YOLOv8 (bounding boxes)
   - Stage 2: License plate segmentation on vehicle ROIs using car-plate-seg model
   - Generate pixel-level segmentation masks for each license plate
   - Temporal tracking (SORT/DeepSORT) to maintain detection IDs across frames
   - Produce detection metadata (segmentation masks with tracking IDs)

3. **Redaction Thread**
   - Apply Gaussian blur only to segmented license plate regions (pixel-accurate)
   - Interpolate segmentation masks between frames for smooth transitions
   - Handle detection appearance/disappearance with fade in/out
   - In-place modification of frame buffer
   - Memory pool for buffer reuse

4. **Video Writer Thread**
   - Encode redacted frames
   - Write to output file
   - Match input codec/bitrate

### Temporal Consistency for Glitch-Free Redaction

**Key Strategies**:
1. **Object Tracking**: Use SORT (Simple Online Realtime Tracking) to maintain detection IDs across frames
2. **Bounding Box Smoothing**: Apply temporal filtering (e.g., moving average) to bounding box coordinates
3. **Interpolation**: For missed detections, interpolate position from previous frames
4. **Fade Transitions**: Gradually increase/decrease blur opacity when detections appear/disappear
5. **Detection History**: Maintain short history (5-10 frames) for each tracked object

### Optimization Strategies

**Detection Optimizations**:
- Run detection every N frames (e.g., every 2-3 frames) for efficiency
- Use temporal tracking to interpolate between detections
- Adaptive detection frequency based on motion analysis

**GPU Acceleration** (optional, with CPU fallback):
- OpenCV CUDA modules for preprocessing
- CUDA/TensorRT for model inference (5-10x speedup)
- Automatic CPU fallback if GPU unavailable

**I/O Optimizations**:
- Async I/O for read/write operations
- Memory-mapped files for large videos
- Hardware-accelerated codec support

## Project Structure

```
license-plate-redactor-v2/
├── CMakeLists.txt                 # Root CMake configuration
├── vcpkg.json                     # Dependency manifest
├── README.md                      # Project documentation
├── LICENSE                        # License file
├── .gitignore                     # Git ignore patterns
├── cmake/
│   ├── FindOpenCV.cmake           # CMake find scripts
│   └── CompilerSettings.cmake     # Compiler flags and options
├── src/
│   ├── core/                      # Core processing library
│   │   ├── VideoProcessor.h       # Main pipeline orchestrator
│   │   ├── VideoProcessor.cpp
│   │   ├── Detector.h             # YOLOv8 model wrapper
│   │   ├── Detector.cpp
│   │   ├── Tracker.h              # Temporal tracking (SORT)
│   │   ├── Tracker.cpp
│   │   ├── Redactor.h             # Gaussian blur redaction logic
│   │   ├── Redactor.cpp
│   │   ├── FrameBuffer.h          # Memory management
│   │   ├── FrameBuffer.cpp
│   │   └── CMakeLists.txt
│   └── cli/                       # Command-line interface
│       ├── main.cpp
│       └── CMakeLists.txt
├── models/                        # Model weights (gitignored, downloaded separately)
│   ├── .gitkeep
│   ├── README.md                  # Model download instructions
│   └── download-models.sh         # Model download script
├── tests/
│   ├── data/                      # Test video files and samples
│   │   ├── .gitkeep
│   │   └── README.md              # Test data description
│   ├── unit/                      # Unit tests
│   │   └── CMakeLists.txt
│   └── integration/               # Integration tests
│       └── CMakeLists.txt
├── docs/
│   ├── ARCHITECTURE.md            # Architecture documentation
│   ├── BUILD.md                   # Build instructions
│   └── USAGE.md                   # Usage guide
└── scripts/
    ├── setup-vcpkg.sh             # vcpkg setup script
    ├── download-models.py         # Python script for model download
    └── install-dependencies.sh    # Dependency installation
```

## Implementation Steps

### Phase 1: Project Setup
1. Initialize Git repository
2. Create CMakeLists.txt with vcpkg integration
3. Create vcpkg.json manifest with dependencies
4. Set up project directory structure
5. Create README.md with build instructions

### Phase 2: Core Video Pipeline
1. Implement basic VideoProcessor class
2. Create frame-by-frame reading with OpenCV VideoCapture
3. Implement VideoWriter for output
4. Test with simple copy operation (input → output)
5. Add memory management with FrameBuffer class

### Phase 3: Detection Integration
1. Download and integrate YOLOv8 ONNX models (vehicle detection + car-plate-seg)
2. Implement Detector class with OpenCV DNN module
3. Add vehicle detection capability (Stage 1)
4. Implement license plate segmentation on vehicle ROIs (Stage 2 with car-plate-seg)
5. Extract and process segmentation masks for precise license plate regions
6. Test detection and segmentation accuracy on sample videos

### Phase 4: Temporal Tracking
1. Implement Tracker class with SORT algorithm
2. Maintain detection IDs across frames
3. Handle detection appearance/disappearance
4. Smooth bounding box coordinates with temporal filtering
5. Test tracking stability on sample videos

### Phase 5: Redaction Logic
1. Implement Redactor class with Gaussian blur
2. Apply blur to segmented license plate regions (pixel-level precision)
3. Implement segmentation mask interpolation for missed frames
4. Add fade transitions for smooth appearance/disappearance
5. Handle edge cases (partial plates, overlapping detections)
6. Test for glitch-free continuous redaction with pixel-accurate blur

### Phase 6: Multi-Threading Pipeline
1. Design thread-safe queue system
2. Implement reader thread
3. Add detection thread pool
4. Create redaction thread
5. Implement writer thread
6. Add synchronization and backpressure control

### Phase 7: CLI Tool
1. Implement command-line argument parsing (cxxopts/CLI11)
2. Add batch processing capability for single files
3. Add batch processing for multiple files/directories
4. Create progress indicators for terminal
5. Add verbose logging options
6. Add configuration options (blur strength, detection threshold, etc.)

### Phase 8: Testing & Optimization
1. Write unit tests for core components
2. Create integration tests with sample videos in tests/data/
3. Profile memory usage and optimize
4. Test with various video formats and resolutions
5. Benchmark processing speed
6. Verify glitch-free redaction across different scenarios

### Phase 9: Documentation
1. Write comprehensive README
2. Create architecture documentation
3. Add build instructions for macOS/Linux
4. Document CLI usage and examples
5. Add troubleshooting guide
6. Document temporal consistency approach

### Phase 10: GitHub Preparation
1. Add appropriate .gitignore
2. Choose and add LICENSE file
3. Create GitHub Actions CI/CD pipeline
4. Add issue templates
5. Create contribution guidelines

## Key Decisions & Implementation Details

### Finalized Decisions

1. **Detection Model**: YOLOv8
   - Balanced accuracy and speed (~30-50ms/frame)
   - Excellent license plate detection capability
   - Strong community support and ONNX export

2. **GPU Acceleration**: Optional with CPU fallback
   - More compatible across different systems
   - CPU performance acceptable for offline batch processing
   - GPU provides 5-10x speedup when available

3. **User Interface**: CLI only (batch processing)
   - Simpler implementation and maintenance
   - Suitable for automated workflows
   - Lower memory footprint
   - Easy to integrate into scripts and pipelines

4. **Redaction Method**: Gaussian blur
   - Harder to reverse engineer than pixelation
   - Natural appearance in video
   - Good balance between privacy and aesthetics
   - Configurable blur strength via CLI parameters

5. **Processing Mode**: Batch offline processing only
   - Simpler architecture
   - Memory efficient
   - Predictable performance
   - No real-time constraints

### Temporal Consistency Implementation

**SORT (Simple Online Realtime Tracking)**:
- Track detected objects frame-to-frame using Kalman filtering
- Assign unique IDs to each detected license plate
- Maintain detection across temporary occlusions

**Bounding Box Smoothing**:
- Apply moving average filter to bounding box coordinates
- Window size: 3-5 frames for responsive tracking
- Prevents jitter in redaction regions

**Interpolation Strategy**:
- If detection missed for 1-3 frames, interpolate position
- Use linear interpolation for short gaps
- Use Kalman prediction for longer gaps (up to 10 frames)
- Beyond 10 frames, treat as new detection

**Fade Transitions**:
- New detections: fade in blur over 3-5 frames (0% → 100% opacity)
- Lost detections: fade out blur over 3-5 frames (100% → 0% opacity)
- Prevents sudden appearance/disappearance artifacts

### Technical Challenges & Solutions

1. **Model Accuracy**
   - Challenge: Handling various plate types, angles, lighting
   - Solution: Use YOLOv8 with car-plate-seg segmentation model for precise plate regions
   - Mitigation: Blur entire segmented region for complete unreadability

2. **Memory Constraints**
   - Challenge: 4K video at 60fps = ~1.5 GB/sec raw data
   - Solution: Frame-level streaming, never decompress full video
   - Monitoring: Track memory usage, fail gracefully near limit

3. **Processing Speed**
   - Challenge: 4K30fps = 33ms budget per frame
   - Reality: Detection may take 30-100ms per frame
   - Solution: Accept slower-than-real-time, show clear progress

4. **Temporal Consistency**
   - Challenge: Maintaining smooth redaction across frames
   - Solution: SORT tracking + bounding box smoothing + interpolation
   - Testing: Visual inspection for any flicker or gaps

5. **Output File Size**
   - Challenge: Re-encoding may increase file size
   - Solution: Match input codec/bitrate settings
   - Trade-off: Allow quality parameter tuning via CLI

6. **Cross-Platform Compatibility**
   - Challenge: Different hardware acceleration on macOS/Linux
   - Solution: Abstract platform-specific code, provide fallbacks
   - Testing: CI/CD pipeline for both platforms

## CLI Interface Design

### Basic Usage
```bash
# Single file processing
./license-plate-redactor -i input.mp4 -o output.mp4

# With options
./license-plate-redactor -i input.mp4 -o output.mp4 \
  --blur-strength 25 \
  --detection-threshold 0.5 \
  --use-gpu
```

### Advanced Options
```bash
Options:
  -i, --input FILE           Input video file (required)
  -o, --output FILE          Output video file (required)
  -m, --model PATH           Path to YOLO model (default: models/yolov8.onnx)
  -b, --blur-strength INT    Gaussian blur kernel size (default: 25)
  -t, --threshold FLOAT      Detection confidence threshold (default: 0.5)
  -g, --use-gpu              Enable GPU acceleration if available
  -v, --verbose              Verbose logging
  -p, --progress             Show progress bar
  -j, --threads INT          Number of detection threads (default: 2)
  -h, --help                 Show this help message
```

### Batch Processing
```bash
# Process multiple files
for file in videos/*.mp4; do
  ./license-plate-redactor -i "$file" -o "redacted_$(basename $file)"
done
```

## Success Criteria

- ✅ Successfully processes 4K MP4 videos up to 10GB
- ✅ Runs on macOS/Linux with 16GB RAM without crashes
- ✅ Detects and redacts multiple license plates per frame
- ✅ Handles various plate orientations and conditions
- ✅ Output file size within 2x input size
- ✅ Processing speed reasonable (1-2x real-time for HD, slower for 4K acceptable)
- ✅ **Continuous glitch-free redaction across all frames**
- ✅ CLI tool with clear options and progress indication
- ✅ Complete documentation and build instructions
- ✅ Ready for GitHub distribution
- ✅ Test data available in tests/data/ directory

## Future Enhancements

- Support additional video formats (AVI, MOV, MKV)
- GUI wrapper for desktop users
- GPU acceleration with CUDA/TensorRT
- Enhanced temporal tracking algorithms (DeepSORT)
- Manual review mode for uncertain detections
- Batch processing optimizations for multiple files
- Settings for redaction intensity/style variations
- Export detection statistics and metadata
- macOS/Linux native installers
- Configuration file support
- Plugin system for custom detectors
