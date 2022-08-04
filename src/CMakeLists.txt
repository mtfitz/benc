include_directories(${BENC_SOURCE_DIR}/include/)
add_library(benc decoder.cpp encoder.cpp tree.cpp)
add_executable(benc-cli benc.cpp)
target_link_libraries(benc-cli benc)