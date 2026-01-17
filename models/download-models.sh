#!/bin/bash
# Script to download YOLOv8 models

set -e

echo "Downloading YOLOv8 models..."

# Check if Python and pip are available
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is required to download models"
    exit 1
fi

# Install ultralytics if not present
if ! python3 -c "import ultralytics" &> /dev/null; then
    echo "Installing ultralytics..."
    pip3 install ultralytics
fi

# Download and export YOLOv8n
echo "Downloading YOLOv8n model..."
python3 << EOF
from ultralytics import YOLO
import os

# Download YOLOv8n
model = YOLO('yolov8n.pt')

# Export to ONNX
model.export(format='onnx')

# Move to current directory
import shutil
if os.path.exists('yolov8n.onnx'):
    print("YOLOv8n model downloaded successfully!")
else:
    print("Error: Model download failed")
EOF

echo ""
echo "Models downloaded successfully!"
echo ""
echo "Note: For license plate detection, you'll need to:"
echo "1. Train a custom YOLOv8 model on license plate data, or"
echo "2. Download a pre-trained model from Roboflow or similar sources"
echo ""
echo "Place the license plate model as 'yolov8-license-plate.onnx' in this directory."
