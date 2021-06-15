/*
    RIAPS Timesync Service

    Copyright (C) Vanderbilt University, ISIS 2016

*/
#include <stdio.h>
#include <stdlib.h>
#include "riaps_ts.h"

const char* role_str[] = {
    "MASTER",
    "SLAVE"
};

const char* reference_str[] = {
    "NONE",
    "GPS",
    "NTP",
    "PTP"
};

int main(int argc, const char* argv[])
{
    struct riaps_ts_timespec now;
    struct riaps_ts_timespec snooze;
    struct riap_ts_status status;

    if (riaps_ts_gettime(&now)) {
        perror("ERROR: riaps_ts_gettime()");
        exit(-1);
    }

    // Try to wake up on second boundaries (PPS)
    snooze.tv_sec = now.tv_sec + 1;
    snooze.tv_nsec = 0;

    for(;;) {
        riaps_ts_gettime(&now);
        printf("wake: %lld.%09ld secs\n", (long long)now.tv_sec, (long)now.tv_nsec);
        if (riaps_ts_status(&status)) {
            perror("ERROR: riaps_ts_status()");
        }
        else {
            printf("\trole: %s\n"
                    "\treference: %s\n"
                    "\tnow: %lld.%09ld secs\n"
                    "\tlast_offset: %.9f secs\n"
                    "\trms_offset: %.9f secs\n"
                    "\tppm: %.6f\n",
                    role_str[status.role],
                    reference_str[status.reference],
                    (long long)status.now.tv_sec, (long)status.now.tv_nsec,
                    status.last_offset,
                    status.rms_offset,
                    status.ppm);
        }

        snooze.tv_sec++;
        riaps_ts_sleep(RIAPS_TS_ABSTIME, &snooze);
    }
    return 0;
}
