# Core Library

This directory contains the core processing components of the License Plate Redactor.

## Components

- **VideoProcessor**: Main pipeline orchestrator
- **Detector**: YOLOv8 model wrapper for vehicle and license plate detection
- **Tracker**: Temporal tracking using SORT algorithm
- **Redactor**: Gaussian blur redaction logic
- **FrameBuffer**: Memory management for efficient frame processing

## Building

This library is built as part of the main CMake project. See the root CMakeLists.txt.

## Usage

The core library is used by the CLI application and can be integrated into other applications.
