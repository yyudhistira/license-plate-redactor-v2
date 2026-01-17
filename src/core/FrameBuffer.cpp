#include "FrameBuffer.h"
#include <algorithm>

namespace lpr {

FrameBuffer::FrameBuffer(size_t capacity)
    : capacity_(capacity) {
}

cv::Mat FrameBuffer::acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!pool_.empty()) {
        cv::Mat frame = pool_.front();
        pool_.pop();
        return frame;
    }
    
    // Return empty Mat - caller will initialize with proper size
    return cv::Mat();
}

void FrameBuffer::release(cv::Mat frame) {
    if (frame.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Only keep up to capacity_ frames in the pool
    if (pool_.size() < capacity_) {
        pool_.push(frame);
    }
    // If pool is full, let the frame be destroyed (memory released)
}

size_t FrameBuffer::available() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pool_.size();
}

void FrameBuffer::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear the queue by swapping with an empty queue
    std::queue<cv::Mat> empty;
    std::swap(pool_, empty);
}

} // namespace lpr
