#!/bin/bash
# Install system dependencies for macOS and Linux

set -e

echo "Installing system dependencies..."

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "Detected macOS"
    
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Please install Homebrew first:"
        echo "https://brew.sh"
        exit 1
    fi
    
    echo "Installing dependencies via Homebrew..."
    brew install cmake pkg-config
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    echo "Detected Linux"
    
    if command -v apt-get &> /dev/null; then
        # Debian/Ubuntu
        echo "Installing dependencies via apt..."
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            pkg-config \
            git \
            curl \
            zip \
            unzip \
            tar
            
    elif command -v yum &> /dev/null; then
        # RedHat/CentOS
        echo "Installing dependencies via yum..."
        sudo yum install -y \
            gcc-c++ \
            cmake \
            pkgconfig \
            git \
            curl \
            zip \
            unzip \
            tar
    else
        echo "Unknown package manager. Please install dependencies manually:"
        echo "- CMake 3.20+"
        echo "- C++20 compiler"
        echo "- pkg-config"
        echo "- git"
        exit 1
    fi
else
    echo "Unsupported operating system: $OSTYPE"
    exit 1
fi

echo ""
echo "✓ System dependencies installed successfully!"
echo ""
echo "Next, run: ./scripts/setup-vcpkg.sh"
