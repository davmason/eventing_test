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
#include <linux/user_events.h>
#include <functional>

TRACELOGGING_DEFINE_PROVIDER(
    MyProvider,
    "MyProviderName",
    // {b7aa4d18-240c-5f41-5852-817dbf477472}
    (0xb7aa4d18, 0x240c, 0x5f41, 0x58, 0x52, 0x81, 0x7d, 0xbf, 0x47, 0x74, 0x72));

const char *data_file = "/sys/kernel/tracing/user_events_data";

// TODO: can share these with different bits
volatile int simple_enabled = 0;
volatile int big_enabled = 0;

static int event_reg(int fd, const char *command, int *write, volatile int *enabled)
{
    struct user_reg reg = {0};

    reg.size = sizeof(reg);
    reg.enable_bit = 31;
    reg.enable_size = sizeof(*enabled);
    reg.enable_addr = (__u64)enabled;
    reg.name_args = (__u64)command;

    if (ioctl(fd, DIAG_IOCSREG, &reg) == -1)
        return -1;

    *write = reg.write_index;

    return 0;
}

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
    int data_fd, simple_write, big_write;
    __u32 count = 0;

    data_fd = open(data_file, O_RDWR);

    if (event_reg(data_fd, "test u32 iteration", &simple_write, &simple_enabled) == -1
        || event_reg(data_fd, "test2 char[1000] array", &big_write, &big_enabled) == -1)
    {
        printf("error user_events: %s\n", strerror(errno));
        return errno;
    }

    // int err = 0;

    // printf("\n");

    // err = TraceLoggingRegister(MyProvider);
    // if (err != 0)
    // {
    //     printf("Error registering MyProvider err=%d\n", err);
    //     return err;
    // }

    // event_level const event1_level = event_level_information;
    // uint64_t const event1_keyword = 0x1;

    // TraceLoggingWrite(
    //     MyProvider,                               // Provider to use for the event.
    //     "SimpleEvent",                                 // Event name.
    //     TraceLoggingLevel(event1_level),          // Event severity level.
    //     TraceLoggingKeyword(event1_keyword),      // Event category bits.
    //     TraceLoggingUInt32(0));           // uint32 field named "iteration".

    printf("Waiting for event to be enabled.\n");
    // while (!TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword))
    while (!simple_enabled)
    {
        // printf("MyProviderName_L4K1 Event1 status=%x\n",
        //     TraceLoggingProviderEnabled(MyProvider, event1_level, event1_keyword));
        printf("user_events simple_enabled=%d\n", simple_enabled);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    printf("Event enabled.\n");

    std::function<void()> simple_work = [&]()
    {
        const int event_count = 500000;
        struct iovec io[2];
        for (unsigned iteration = 1; iteration <= event_count; iteration += 1)
        {
            io[0].iov_base = &simple_write;
            io[0].iov_len = sizeof(simple_write);
            io[1].iov_base = &iteration;
            io[1].iov_len = sizeof(iteration);

            if (writev(data_fd, (const struct iovec *)io, 2) == -1)
            {
                printf("Error writing event %s\n", strerror(errno));
                return;
            }
            // TraceLoggingWrite(
            //     MyProvider,                               // Provider to use for the event.
            //     "SimpleEvent",                                 // Event name.
            //     TraceLoggingLevel(event1_level),          // Event severity level.
            //     TraceLoggingKeyword(event1_keyword),      // Event category bits.
            //     TraceLoggingUInt32(iteration));           // uint32 field named "iteration".
        }
    };
    time_it(simple_work, "Firing 500,000 simple events");

    printf("What?\n");
    std::function<void()> big_work = [&]()
    {
        const int event_count = 500000;
        struct iovec io[2];
        char[1000] data_buffer;
        memset(data_buffer, 11, 1000);

        for (unsigned iteration = 1; iteration <= event_count; iteration += 1)
        {
            io[0].iov_base = &big_write;
            io[0].iov_len = sizeof(big_write);
            io[1].iov_base = data_buffer;
            io[1].iov_len = 1000;

            if (writev(data_fd, (const struct iovec *)io, 2) == -1)
            {
                printf("Error writing event %s\n", strerror(errno));
                return;
            }
            // TraceLoggingWrite(
            //     MyProvider,                               // Provider to use for the event.
            //     "SimpleEvent",                                 // Event name.
            //     TraceLoggingLevel(event1_level),          // Event severity level.
            //     TraceLoggingKeyword(event1_keyword),      // Event category bits.
            //     TraceLoggingUInt32(iteration));           // uint32 field named "iteration".
        }
    };
    time_it(big_work, "Firing 500,000 big events");

    // TraceLoggingUnregister(MyProvider);
    // return err;
    return 0;
}