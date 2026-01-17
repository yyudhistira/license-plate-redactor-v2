# Test Data Directory

Place test video files in this directory for testing the license plate redaction system.

## Test Video Requirements

- **Format**: MP4 (H.264 codec recommended)
- **Resolution**: Various (720p, 1080p, 4K)
- **Content**: Videos containing vehicles with visible license plates

## Suggested Test Cases

1. **basic_highway.mp4** - Highway traffic with multiple vehicles
2. **parking_lot.mp4** - Stationary vehicles in parking lot
3. **city_traffic.mp4** - Urban traffic with various angles
4. **4k_test.mp4** - 4K resolution video for performance testing

## Data Sources

Test videos can be obtained from:
- [Pexels](https://www.pexels.com/search/videos/traffic/) - Free stock videos
- [Pixabay](https://pixabay.com/videos/search/cars/) - Free videos
- Your own dashcam footage (ensure privacy compliance)

## Privacy Notice

⚠️ **Important**: Do not commit videos containing real, identifiable license plates to the repository. Use synthetic test data or ensure plates are already obscured in test videos.

## File Organization

```
tests/data/
├── input/           # Original test videos
├── output/          # Processed output videos
└── ground_truth/    # Expected results for validation
```

All video files are gitignored to keep the repository size manageable.
