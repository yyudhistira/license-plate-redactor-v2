#pragma once

#include "FrameBuffer.h"
#include <opencv2/videoio.hpp>
#include <string>
#include <functional>

namespace lpr {

/**
 * @brief Processing statistics
 */
struct ProcessingStats {
    int total_frames = 0;
    int processed_frames = 0;
    double fps = 0.0;
    double elapsed_seconds = 0.0;
    
    [[nodiscard]] double progress() const {
        return total_frames > 0 ? static_cast<double>(processed_frames) / total_frames : 0.0;
    }
};

/**
 * @brief Main video processing pipeline
 * 
 * Handles frame-by-frame video reading, processing, and writing.
 * Designed for memory efficiency with large videos.
 */
class VideoProcessor {
public:
    /**
     * @brief Callback function for frame processing
     * 
     * @param frame Input frame (read-only)
     * @param frame_number Current frame number (0-indexed)
     * @return cv::Mat Processed frame to write (can be same as input)
     */
    using FrameCallback = std::function<cv::Mat(const cv::Mat& frame, int frame_number)>;
    
    /**
     * @brief Callback function for progress updates
     * 
     * @param stats Current processing statistics
     */
    using ProgressCallback = std::function<void(const ProcessingStats& stats)>;
    
    /**
     * @brief Construct a new Video Processor
     */
    VideoProcessor();
    
    ~VideoProcessor() = default;
    
    // Disable copy and move (contains FrameBuffer with mutex)
    VideoProcessor(const VideoProcessor&) = delete;
    VideoProcessor& operator=(const VideoProcessor&) = delete;
    VideoProcessor(VideoProcessor&&) = delete;
    VideoProcessor& operator=(VideoProcessor&&) = delete;

    /**
     * @brief Open input video file
     * 
     * @param input_path Path to input video file
     * @return true if opened successfully
     */
    bool openInput(const std::string& input_path);
    
    /**
     * @brief Open output video file
     * 
     * @param output_path Path to output video file
     * @param fourcc FourCC codec (0 = copy from input)
     * @param fps Frame rate (0 = copy from input)
     * @param quality Quality parameter (0-100, higher = better quality, 0 = default)
     * @return true if opened successfully
     */
    bool openOutput(const std::string& output_path, int fourcc = 0, double fps = 0.0, int quality = 0);
    
    /**
     * @brief Process the video frame by frame
     * 
     * @param callback Function to process each frame (optional, defaults to copy)
     * @param progress_callback Function called for progress updates (optional)
     * @return true if processing completed successfully
     */
    bool process(FrameCallback callback = nullptr, 
                 ProgressCallback progress_callback = nullptr);
    
    /**
     * @brief Get input video properties
     */
    int getFrameWidth() const { return frame_width_; }
    int getFrameHeight() const { return frame_height_; }
    double getFPS() const { return fps_; }
    int getTotalFrames() const { return total_frames_; }
    int getFourCC() const { return fourcc_; }
    
    /**
     * @brief Get processing statistics
     */
    const ProcessingStats& getStats() const { return stats_; }
    
    /**
     * @brief Close video files and release resources
     */
    void close();

private:
    // Video I/O
    cv::VideoCapture video_capture_;
    cv::VideoWriter video_writer_;
    
    // Video properties
    int frame_width_ = 0;
    int frame_height_ = 0;
    double fps_ = 0.0;
    int total_frames_ = 0;
    int fourcc_ = 0;
    
    // Processing
    FrameBuffer frame_buffer_;
    ProcessingStats stats_;
    
    // Helper methods
    bool readVideoProperties();
};

} // namespace lpr
