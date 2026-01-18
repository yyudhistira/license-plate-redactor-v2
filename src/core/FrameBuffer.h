#pragma once

#include <opencv2/core.hpp>
#include <queue>
#include <mutex>

namespace lpr {

/**
 * @brief Memory-efficient frame buffer manager
 * 
 * Provides pre-allocated cv::Mat objects for frame processing to avoid
 * repeated memory allocation/deallocation. Uses a pool pattern for reuse.
 */
class FrameBuffer {
public:
    /**
     * @brief Construct a new Frame Buffer
     * 
     * @param capacity Maximum number of frames to keep in the buffer pool
     */
    explicit FrameBuffer(size_t capacity = 16);
    
    ~FrameBuffer() = default;
    
    // Disable copy and move (contains mutex)
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&) = delete;
    FrameBuffer& operator=(FrameBuffer&&) = delete;

    /**
     * @brief Acquire a frame buffer from the pool
     * 
     * Returns an existing buffer if available, or creates a new one.
     * The buffer may contain old data and should be overwritten.
     * 
     * @return cv::Mat Frame buffer (may be empty if not yet initialized)
     */
    cv::Mat acquire();
    
    /**
     * @brief Return a frame buffer to the pool for reuse
     * 
     * @param frame Frame to return to the pool
     */
    void release(cv::Mat frame);
    
    /**
     * @brief Get the current number of available buffers in the pool
     * 
     * @return size_t Number of available buffers
     */
    size_t available() const;
    
    /**
     * @brief Clear all buffers and reset the pool
     */
    void clear();

private:
    size_t capacity_;
    std::queue<cv::Mat> pool_;
    mutable std::mutex mutex_;
};

} // namespace lpr
