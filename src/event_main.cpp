// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <eventheader/TraceLoggingProvider.h>

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <functional>

TRACELOGGING_DEFINE_PROVIDER(
    MyProvider,
    "MyProviderName",
    // {b7aa4d18-240c-5f41-5852-817dbf477472}
    (0xb7aa4d18, 0x240c, 0x5f41, 0x58, 0x52, 0x81, 0x7d, 0xbf, 0x47, 0x74, 0x72));

void time_it(std::function<void()> work, std::string description)
{
    const auto start = std::chrono::system_clock::now();

    work();

    const auto end = std::chrono::system_clock::now();
    const auto diff = end - start;

    printf("%s: %dms\n", description.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
}

int main()
{
    int err = 0;

    printf("\n");

    err = TraceLoggingRegister(MyProvider);
    if (err != 0)
    {
        printf("Error registering MyProvider err=%d\n", err);
        return err;
    }

    event_level const event1_level = event_level_information;
    uint64_t const event1_keyword = 0x1;

    TraceLoggingWrite(
        MyProvider,                               // Provider to use for the event.
        "SimpleEvent",                                 // Event name.
        TraceLoggingLevel(event1_level),          // Event severity level.
        TraceLoggingKeyword(event1_keyword),      // Event category bits.
        TraceLoggingUInt32(0));           // uint32 field named "iteration".

    printf("Waiting for event to be enabled.\n");
    while (!TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword))
    // while (!simple_enabled)
    {
        printf("MyProviderName_L4K1 Event1 status=%x\n",
            TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword));
        // printf("user_events simple_enabled=%d\n", simple_enabled);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    printf("Event enabled.\n");

    std::function<void()> simple_work = [&]()
    {
        const int event_count = 500000;
        for (unsigned iteration = 1; iteration <= event_count; iteration += 1)
        {
            TraceLoggingWrite(
                MyProvider,
                "SimpleEvent",
                TraceLoggingLevel(event1_level),
                TraceLoggingKeyword(event1_keyword),
                TraceLoggingUInt32(iteration));
        }
    };
    time_it(simple_work, "Firing 500,000 simple events");

    std::function<void()> big_work = [&]()
    {
        int32_t data_buffer[1000];
        memset(data_buffer, 11, 1000);
        uint8_t const guid[16] = { 1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};

        const int event_count = 500000;
        for (unsigned iteration = 1; iteration <= event_count; iteration += 1)
        {
            TraceLoggingWrite(
                MyProvider, 
                "ComplexEvent",
                TraceLoggingLevel(event1_level),
                TraceLoggingKeyword(event1_keyword),
                TraceLoggingUInt32(iteration),
                TraceLoggingInt32Array(data_buffer, 1000),
                TraceLoggingGuid(guid));
        }
    };
    time_it(big_work, "Firing 500,000 complex events");

    TraceLoggingUnregister(MyProvider);
    return err;
}
