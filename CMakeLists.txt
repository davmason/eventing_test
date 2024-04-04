cmake_minimum_required (VERSION 3.14)

project(eventing_test)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wno-invalid-noreturn
                    -Wno-pragma-pack
                    -Wno-int-to-pointer-cast
                    -fPIC
                    -fms-extensions
                    -DBIT64
                    -DPAL_STDCPP_COMPAT
                    -DPLATFORM_UNIX
                    -DHOST_64BIT)

include_directories(src/libtracepoint/inc)

add_link_options(--no-undefined -lpthread)

set(EVENT_SOURCES
    src/event_main.cpp
    src/libtracepoint/eventheader-tracepoint.c
    src/libtracepoint/tracepoint.c)

add_executable(eventing_test ${EVENT_SOURCES})

set(COLLECT_SOURCES
    src/collect_main.cpp
    src/libtracepoint/TracepointCache.cpp
    src/libtracepoint/TracepointPath.cpp
    src/libtracepoint/TracePointSession.cpp
    src/libtracepoint/UniqueHandles.cpp
    src/libtracepoint/PerfByteReader.cpp
    src/libtracepoint/PerfDataFile.cpp
    src/libtracepoint/PerfDataFileWrapper.cpp
    src/libtracepoint/PerfEventAbi.cpp
    src/libtracepoint/PerfEventInfo.cpp
    src/libtracepoint/PerfEventMetadata.cpp
    src/libtracepoint/PerfEventSessionInfo.cpp)

add_executable(eventing_collect ${COLLECT_SOURCES})