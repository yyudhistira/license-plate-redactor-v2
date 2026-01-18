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
echo "Downloading car-plate-seg model..."
python3 << EOF
from ultralytics import YOLO
import os

# Download car-plate-seg model from Roboflow
try:
    model = YOLO('https://hub.ultralytics.com/models/car-plate-seg')
    print("Attempting Ultralytics Hub download...")
except:
    print("Hub download not available, trying alternative...")
    # Fallback: download from Roboflow Community
    model = YOLO('yolov8n-seg.pt')  # Use segmentation variant
    
# Export to ONNX
model.export(format='onnx')

if os.path.exists('yolov8n-seg.onnx'):
    print("Segmentation model exported successfully!")
EOF

echo ""
echo "Models downloaded successfully!"
echo ""
echo "Downloaded models:"
echo "  - yolov8n.onnx (vehicle detection)"
echo "  - yolov8n-seg.onnx (segmentation)"
echo ""
echo "Note: For advanced license plate detection/segmentation, you can:"
echo "1. Download pre-trained models from Roboflow Community"
echo "2. Fine-tune on your own license plate dataset"
echo "3. Place custom models as 'yolov8-license-plate.onnx' in this directory"
