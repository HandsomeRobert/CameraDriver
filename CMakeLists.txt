# CMakeList.txt: CameraDriver 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
project ("CameraDriver")

set(CMAKE_CXX_STANDARD 11)
set(_HAS_STD_BYTE 0)
option(OPENCV "Opencv Interface" ON)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(jsoncpp CONFIG REQUIRED)
include_directories("$ENV{VCPKG_ROOT}/installed/x64-windows/include")

include_directories("$ENV{MVCAM_COMMON_RUNENV}/Includes")
include_directories("./lib/")

link_directories("$ENV{MVCAM_COMMON_RUNENV}/Libraries/win64")
link_directories("./lib")

aux_source_directory(utils/common COMMON_DIR)
aux_source_directory(src SRC_DIR)
aux_source_directory(Camera CAMERA_DIR)
aux_source_directory(utils UTILS_DIR)

link_libraries(jsoncpp_lib jsoncpp_object)#Static Link

#add_library(${PROJECT_NAME} SHARED ${CAMERA_DIR} ${IMAGE_DIR} ${UTILS_DIR})
add_executable(${PROJECT_NAME} ${COMMON_DIR} ${SRC_DIR} ${CAMERA_DIR} ${UTILS_DIR})##${SRC_DIR}
target_link_libraries(${PROJECT_NAME} MvCameraControl)
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS})
