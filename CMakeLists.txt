cmake_minimum_required(VERSION 3.22)
project(yolov8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release) # None, Debug, Release, RelWithDebInfo, MinSizeRel
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wno-unused-parameter -O0 -g -Wall")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS}  -Wno-unused-parameter -O0 -g -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wno-unused-parameter -O3 -g -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS}  -Wno-unused-parameter -O3 -g -Wall")

include_directories(include)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(RKNNRT_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rknn_lib)  
link_directories(${RKNNRT_LIB_PATH}) #使用 link_directories 明确指定 rknnrt 库所在的目录

# 指定 ByteTrack 头文件路径
include_directories(/home/topeet/YIBO/project/rk3588-yolo-demo-master/ByteTrack/deploy/ByteTrackLib/include)
# 添加 libbytetrack.so 的搜索路径
link_directories(/home/topeet/YIBO/project/rk3588-yolo-demo-master/ByteTrack/deploy/ByteTrackLib/build)
# 指定要链接的 ByteTrack 动态库
set(BYTETRACK_LIBS bytetrack)

add_subdirectory(utils)

find_package(kaylordut REQUIRED)
#find_package(bytetrack REQUIRED)


add_executable(videofile_demo videofile_demo.cpp)
target_link_libraries(videofile_demo ${kaylordut_LIBS} ${OpenCV_LIBS} yolov8-kaylordut ${Bytetrack_LIBS} rknnrt)

add_executable(camera_demo camera_demo.cpp)
target_link_libraries(camera_demo ${kaylordut_LIBS} ${OpenCV_LIBS} yolov8-kaylordut ${Bytetrack_LIBS} ${RKNNRT_LIB_PATH} rknnrt)

add_executable(imagefile_demo imagefile_demo.cpp)
target_link_libraries(imagefile_demo ${kaylordut_LIBS} ${OpenCV_LIBS} yolov8-kaylordut ${Bytetrack_LIBS} ${RKNNRT_LIB_PATH} rknnrt)
