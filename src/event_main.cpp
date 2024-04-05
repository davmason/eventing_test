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

TRACELOGGING_DEFINE_PROVIDER(
    MyProvider,
    "MyProviderName",
    // {b7aa4d18-240c-5f41-5852-817dbf477472}
    (0xb7aa4d18, 0x240c, 0x5f41, 0x58, 0x52, 0x81, 0x7d, 0xbf, 0x47, 0x74, 0x72));

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

    printf("Waiting for provider to be enabled.\n");
    while (!TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword))
    {
        printf("MyProviderName_L4K1 Event1 status=%x\n",
            TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword));
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    printf("Provider enabled.\n");

    const int event_count = 500000;
    const auto start = std::chrono::high_resolution_clock::now();
    for (unsigned iteration = 1; iteration <= event_count; iteration += 1)
    {
        TraceLoggingWrite(
            MyProvider,                               // Provider to use for the event.
            "SimpleEvent",                                 // Event name.
            TraceLoggingLevel(event1_level),          // Event severity level.
            TraceLoggingKeyword(event1_keyword),      // Event category bits.
            TraceLoggingUInt32(iteration));           // uint32 field named "iteration".
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto diff = end - start;

    const double milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / 1000.0;

    printf("Time to fire %d simple events: %f\n", event_count, seconds);
    printf("Debug: diff.ms=%d\n", std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());

    TraceLoggingUnregister(MyProvider);
    return err;
}