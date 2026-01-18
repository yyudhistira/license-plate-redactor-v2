#!/bin/bash
# Copy ONNX models to executable directories

echo "Copying ONNX models to build directories..."

# Copy to debug build
mkdir -p cmake-build-debug/src/cli
cp -v models/*.onnx cmake-build-debug/src/cli/

# Copy to release build
mkdir -p cmake-build-release/src/cli
cp -v models/*.onnx cmake-build-release/src/cli/

echo "Done! Models copied successfully."
echo ""
echo "Debug build models:"
ls -lh cmake-build-debug/src/cli/*.onnx
echo ""
echo "Release build models:"
ls -lh cmake-build-release/src/cli/*.onnx
