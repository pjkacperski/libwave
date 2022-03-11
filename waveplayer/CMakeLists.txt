set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
add_executable(waveplayer waveplayer.cpp)
target_link_libraries(waveplayer wave asound)