cmake_minimum_required (VERSION 2.6)
project (video_streamer CXX)

set(SUPPORTED_PLATFORMS "Linux" "Bebop")

if (";${SUPPORTED_PLATFORMS};" MATCHES ";${PLATFORM};")
    add_executable(${PROJECT_NAME} video_streamer.cxx)
    add_custom_target(${PROJECT_NAME}_headers SOURCES ${PLATFORM_CONFIG})
    target_link_libraries(${PROJECT_NAME} tuv)
endif ()
