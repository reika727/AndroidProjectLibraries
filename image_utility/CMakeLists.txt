set(CMAKE_CXX_STANDARD 17)
add_library(image_utility SHARED image_utility.cpp)
target_link_libraries(image_utility -landroid)