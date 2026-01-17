# Model Files

This directory contains the YOLOv8 ONNX model files for vehicle and license plate detection.

## Required Models

1. **yolov8n.onnx** - YOLOv8 Nano model for general vehicle detection
2. **yolov8-license-plate.onnx** - Fine-tuned YOLOv8 model for license plate detection

## Download Instructions

Run the download script:

```bash
./download-models.sh
```

Or manually download:

### Option 1: Official YOLOv8 Models

```bash
# Install ultralytics (requires Python)
pip install ultralytics

# Export YOLOv8n to ONNX
python -c "from ultralytics import YOLO; model = YOLO('yolov8n.pt'); model.export(format='onnx')"
```

### Option 2: Pre-trained License Plate Models

Pre-trained license plate detection models can be obtained from:
- [Roboflow Universe](https://universe.roboflow.com/) - Search for "license plate detection"
- [Ultralytics Hub](https://hub.ultralytics.com/) - Custom trained models
- Train your own using the YOLOv8 framework

## Model Format

All models should be in ONNX format (.onnx) for cross-platform compatibility.

## Model Sizes

- yolov8n.onnx: ~6 MB
- yolov8-license-plate.onnx: ~6-10 MB (depending on training)

Total: ~12-16 MB

These files are gitignored to keep the repository lightweight.
