cmake_minimum_required (VERSION 3.14)

project(eventing_test)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(LinuxTracepoints)

add_compile_options(-Wno-invalid-noreturn
                    -Wno-pragma-pack
                    -Wno-int-to-pointer-cast)

include_directories(LinuxTracepoints/libtracepoint/include)
include_directories(LinuxTracepoints/libeventheader-tracepoint/include)
include_directories(LinuxTracepoints/libtracepoint-control-cpp/include)
include_directories(LinuxTracepoints/libtracepoint-decode-cpp/include)

add_link_options(--no-undefined -lpthread)

set(EVENT_SOURCES
    src/event_main.cpp)

add_executable(eventing_test ${EVENT_SOURCES})

set(COLLECT_SOURCES
    src/collect_main.cpp)

add_executable(eventing_collect ${COLLECT_SOURCES})
target_link_libraries(eventing_collect tracepoint-control)