#include "VideoProcessor.h"
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <iostream>

namespace lpr {

VideoProcessor::VideoProcessor()
    : frame_buffer_(16) {  // Pre-allocate pool for 16 frames
}

bool VideoProcessor::openInput(const std::string& input_path) {
    video_capture_.open(input_path);
    
    if (!video_capture_.isOpened()) {
        std::cerr << "Error: Could not open input video: " << input_path << std::endl;
        return false;
    }
    
    if (!readVideoProperties()) {
        std::cerr << "Error: Could not read video properties" << std::endl;
        video_capture_.release();
        return false;
    }
    
    std::cout << "Input video opened: " << input_path << std::endl;
    std::cout << "  Resolution: " << frame_width_ << "x" << frame_height_ << std::endl;
    std::cout << "  FPS: " << fps_ << std::endl;
    std::cout << "  Total frames: " << total_frames_ << std::endl;
    
    return true;
}

bool VideoProcessor::openOutput(const std::string& output_path, int fourcc, double fps, int quality) {
    if (!video_capture_.isOpened()) {
        std::cerr << "Error: Input video must be opened before output" << std::endl;
        return false;
    }
    
    // Use input properties if not specified
    if (fourcc == 0) {
        fourcc = fourcc_;
    }
    if (fps <= 0.0) {
        fps = fps_;
    }
    
    cv::Size frame_size(frame_width_, frame_height_);
    
    // Open the video writer
    // Note: Quality control in OpenCV VideoWriter is limited with the simple API
    // The VideoWriter will use codec default settings which often result in higher quality
    // (and thus larger file size) than the input video
    video_writer_.open(output_path, fourcc, fps, frame_size, true);
    
    if (!video_writer_.isOpened()) {
        std::cerr << "Error: Could not open output video: " << output_path << std::endl;
        return false;
    }
    
    std::cout << "Output video opened: " << output_path << std::endl;
    
    return true;
}

bool VideoProcessor::process(FrameCallback callback, ProgressCallback progress_callback) {
    if (!video_capture_.isOpened()) {
        std::cerr << "Error: Input video not opened" << std::endl;
        return false;
    }
    
    if (!video_writer_.isOpened()) {
        std::cerr << "Error: Output video not opened" << std::endl;
        return false;
    }
    
    // Reset statistics
    stats_ = ProcessingStats();
    stats_.total_frames = total_frames_;
    
    auto start_time = std::chrono::steady_clock::now();
    
    int frame_number = 0;
    cv::Mat frame;
    
    while (true) {
        // Read frame
        if (!video_capture_.read(frame)) {
            // End of video or error
            break;
        }
        
        if (frame.empty()) {
            break;
        }
        
        // Process frame
        cv::Mat processed_frame;
        if (callback) {
            processed_frame = callback(frame, frame_number);
        } else {
            // Default: copy frame
            processed_frame = frame;
        }
        
        // Write frame
        if (!processed_frame.empty()) {
            video_writer_.write(processed_frame);
            stats_.processed_frames++;
        }
        
        frame_number++;
        
        // Update progress
        if (progress_callback && frame_number % 30 == 0) {
            auto current_time = std::chrono::steady_clock::now();
            stats_.elapsed_seconds = std::chrono::duration<double>(current_time - start_time).count();
            progress_callback(stats_);
        }
        
        // Release frame back to buffer pool
        frame_buffer_.release(frame);
    }
    
    // Final statistics
    auto end_time = std::chrono::steady_clock::now();
    stats_.elapsed_seconds = std::chrono::duration<double>(end_time - start_time).count();
    
    if (stats_.elapsed_seconds > 0) {
        stats_.fps = stats_.processed_frames / stats_.elapsed_seconds;
    }
    
    std::cout << "\nProcessing complete!" << std::endl;
    std::cout << "  Frames processed: " << stats_.processed_frames << std::endl;
    std::cout << "  Time elapsed: " << stats_.elapsed_seconds << " seconds" << std::endl;
    std::cout << "  Processing FPS: " << stats_.fps << std::endl;
    
    return stats_.processed_frames > 0;
}

void VideoProcessor::close() {
    if (video_capture_.isOpened()) {
        video_capture_.release();
    }
    
    if (video_writer_.isOpened()) {
        video_writer_.release();
    }
    
    frame_buffer_.clear();
}

bool VideoProcessor::readVideoProperties() {
    frame_width_ = static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    frame_height_ = static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    fps_ = video_capture_.get(cv::CAP_PROP_FPS);
    total_frames_ = static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_COUNT));
    fourcc_ = static_cast<int>(video_capture_.get(cv::CAP_PROP_FOURCC));
    
    // Validate properties
    if (frame_width_ <= 0 || frame_height_ <= 0) {
        std::cerr << "Error: Invalid frame dimensions" << std::endl;
        return false;
    }
    
    if (fps_ <= 0.0) {
        std::cerr << "Warning: Invalid FPS, using default 30.0" << std::endl;
        fps_ = 30.0;
    }
    
    // total_frames might be 0 for some formats (e.g., streams)
    if (total_frames_ <= 0) {
        std::cerr << "Warning: Cannot determine total frame count" << std::endl;
    }
    
    return true;
}

} // namespace lpr
