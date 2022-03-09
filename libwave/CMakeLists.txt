set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
add_library(wave wave/WaveFileBase.cpp wave/WaveFileUtil.cpp AudioFileUtil.cpp)
