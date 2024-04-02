cmake_minimum_required (VERSION 3.14)

project(eventing_test)

add_compile_options(-Wno-invalid-noreturn
                    -Wno-pragma-pack
                    -Wno-int-to-pointer-cast
                    -fPIC
                    -fms-extensions
                    -DBIT64
                    -DPAL_STDCPP_COMPAT
                    -DPLATFORM_UNIX
                    -DHOST_64BIT
                    -std=c++17)

include_directories(src/libeventheader-tracepoint/inc)

add_link_options(--no-undefined -lpthread -lunwind)

set(SOURCES
    src/main.cpp
    src/libeventheader-tracepoint/eventheader-tracepoint.c)

add_executable(eventing_test SHARED ${SOURCES})