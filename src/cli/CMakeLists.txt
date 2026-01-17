cmake_minimum_required(VERSION 3.20)

# CLI executable
add_executable(license-plate-redactor main.cpp)

# Link core library
target_link_libraries(license-plate-redactor
    PRIVATE
        lpr_core
        ${OpenCV_LIBS}
)

# Set compile features
target_compile_features(license-plate-redactor PRIVATE cxx_std_20)

# Installation (optional)
install(TARGETS license-plate-redactor
    RUNTIME DESTINATION bin
)
