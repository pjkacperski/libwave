set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
add_executable(wplayer wplayer.cpp)
target_link_libraries(wplayer wave asound)