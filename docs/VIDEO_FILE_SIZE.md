# Output Video File Size Issue

## Problem

The output video file is approximately 2x larger than the input video (60MB vs 32MB for test case).

## Root Cause

**OpenCV VideoWriter Limitations:**

1. **No Direct Bitrate Control**: OpenCV's simple `VideoWriter::open()` API does not provide direct bitrate control parameters
2. **Default Quality Settings**: When copying the codec (FourCC), VideoWriter uses default encoder settings which often prioritize quality over file size
3. **Re-encoding Overhead**: Any re-encoding introduces overhead - even "lossless" copy operations through VideoWriter involve decode/encode cycles

## Why This Happens

When we call:
```cpp
video_writer_.open(output_path, fourcc, fps, frame_size, true);
```

OpenCV/FFmpeg uses **default encoding parameters** for the codec, which typically means:
- Higher quality preset (e.g., CRF 23 for H.264 instead of input's possibly higher CRF)
- Different compression settings
- No consideration of the input video's original bitrate

## Current Workarounds

### Option 1: Accept Larger Files (Current Implementation)
- **Pros**: Simpler code, better quality preservation
- **Cons**: 2x file size increase
- **When to use**: Quality is more important than file size

### Option 2: Use Lower Resolution Processing
- Process at 1080p instead of 4K
- **Pros**: Much smaller files, faster processing
- **Cons**: Quality loss

### Option 3: External Post-Processing
After generating the output video, use FFmpeg directly to compress:
```bash
ffmpeg -i output.mp4 -c:v libx264 -crf 23 -preset medium output_compressed.mp4
```

## Future Solutions

### Advanced VideoWriter Setup (Not Currently Implemented)
OpenCV 4.x VideoWriter supports additional parameters via `VideoWriter::set()` but these must be called BEFORE opening:

```cpp
// This API is complex and codec-specific:
std::vector<int> params = {
    cv::VIDEOWRITER_PROP_QUALITY, 75,  // Quality (codec-dependent)
    cv::VIDEOWRITER_PROP_HW_ACCELERATION, cv::VIDEO_ACCELERATION_ANY
};
video_writer_.open(output_path, fourcc, fps, frame_size, params);
```

However, this is **not well-supported** across all platforms and codecs.

### Best Long-term Solution: FFmpeg Direct Integration
Instead of OpenCV VideoWriter, use FFmpeg's libavcodec directly:
- Full control over bitrate, CRF, presets
- Better compression efficiency
- More complex implementation

## Recommendations

### For Development/Testing
- Accept the larger file sizes (quality 60 setting is reasonable)
- Focus on detection and redaction functionality first

### For Production
- Add post-processing step with FFmpeg to match input bitrate
- Or implement direct FFmpeg integration for full control

## File Size Comparison

| Video | Resolution | Frames | Input Size | Output Size | Ratio |
|-------|------------|--------|------------|-------------|-------|
| Many.mp4 | 3840x2160 | 119 | 32 MB | 60 MB | 1.9x |

## Expected Behavior

For a typical 4K video:
- **Input**: ~50-100 Mbps bitrate (H.264 with CRF 23-28)
- **Output** (our current implementation): ~100-150 Mbps bitrate (H.264 with default settings)
- **Target**: Match input bitrate (requires FFmpeg direct control)

## Next Steps

1. ✅ Document the limitation
2. ⏭️ Implement detection and redaction (Phase 3-5)
3. ⏭️ Add optional FFmpeg post-processing step
4. ⏭️ Consider direct FFmpeg integration for advanced users

## Note

This is a known limitation of OpenCV's VideoWriter API. The same issue affects most OpenCV video processing applications. The trade-off is between simplicity (current approach) and file size optimization (requires more complex FFmpeg integration).

For license plate redaction, **privacy and quality are more important than file size**, so the current approach is acceptable.
