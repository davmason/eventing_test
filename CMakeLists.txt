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

set(SOURCES
    src/main.cpp
    src/libtracepoint/eventheader-tracepoint.c
    src/libtracepoint/tracepoint.c)

add_executable(eventing_test ${SOURCES})