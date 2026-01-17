#include "VideoProcessor.h"
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    std::cout << "License Plate Redactor - Test Pipeline" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    
    // Simple argument parsing
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_video>" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Example:" << std::endl;
        std::cerr << "  " << argv[0] << " input.mp4 output.mp4" << std::endl;
        return 1;
    }
    
    std::string input_path = argv[1];
    std::string output_path = argv[2];
    
    // Create processor
    lpr::VideoProcessor processor;
    
    // Open input video
    if (!processor.openInput(input_path)) {
        std::cerr << "Failed to open input video" << std::endl;
        return 1;
    }
    
    // Open output video (copy codec and fps from input)
    // Use quality 60 for reasonable compression (prevents file bloat)
    // Quality: 0-100, where 100 = best quality (largest file), 60 = good balance
    if (!processor.openOutput(output_path, 0, 0.0, 60)) {
        std::cerr << "Failed to open output video" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Processing video..." << std::endl;
    
    // Progress callback
    auto progress_callback = [](const lpr::ProcessingStats& stats) {
        std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
                  << (stats.progress() * 100.0) << "% "
                  << "(" << stats.processed_frames;
        if (stats.total_frames > 0) {
            std::cout << "/" << stats.total_frames;
        }
        std::cout << " frames, " 
                  << std::setprecision(2) << stats.elapsed_seconds << "s)"
                  << std::flush;
    };
    
    // Process video (default: copy frames without modification)
    bool success = processor.process(nullptr, progress_callback);
    
    std::cout << std::endl;
    
    // Close resources
    processor.close();
    
    if (success) {
        std::cout << "\n✓ Video processing completed successfully!" << std::endl;
        std::cout << "Output saved to: " << output_path << std::endl;
        return 0;
    } else {
        std::cerr << "\n✗ Video processing failed" << std::endl;
        return 1;
    }
}
