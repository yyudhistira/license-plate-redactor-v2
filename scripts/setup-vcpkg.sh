#!/bin/bash
# Setup script for vcpkg and dependencies

set -e

echo "==================================="
echo "License Plate Redactor - Setup"
echo "==================================="
echo ""

# Check if vcpkg is installed
if [ -z "$VCPKG_ROOT" ]; then
    echo "VCPKG_ROOT environment variable is not set."
    echo ""
    read -p "Would you like to install vcpkg? (y/n) " -n 1 -r
    echo ""
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        # Install vcpkg
        INSTALL_DIR="$HOME/vcpkg"
        
        if [ -d "$INSTALL_DIR" ]; then
            echo "vcpkg directory already exists at $INSTALL_DIR"
        else
            echo "Cloning vcpkg..."
            git clone https://github.com/Microsoft/vcpkg.git "$INSTALL_DIR"
        fi
        
        echo "Bootstrapping vcpkg..."
        cd "$INSTALL_DIR"
        ./bootstrap-vcpkg.sh
        
        echo ""
        echo "vcpkg installed successfully!"
        echo ""
        echo "Add this to your shell profile (~/.bashrc, ~/.zshrc, etc.):"
        echo "export VCPKG_ROOT=$INSTALL_DIR"
        echo "export PATH=\$VCPKG_ROOT:\$PATH"
        echo ""
        
        # Set for current session
        export VCPKG_ROOT="$INSTALL_DIR"
        export PATH="$VCPKG_ROOT:$PATH"
    else
        echo "vcpkg installation skipped. Please install vcpkg manually and set VCPKG_ROOT."
        exit 1
    fi
else
    echo "✓ vcpkg found at: $VCPKG_ROOT"
fi

echo ""
echo "vcpkg setup complete!"
echo ""
echo "Next steps:"
echo "1. Build the project:"
echo "   mkdir build && cd build"
echo "   cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "   cmake --build ."
echo ""
echo "2. Download models:"
echo "   cd models && ./download-models.sh"
echo ""
