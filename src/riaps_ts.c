/****************************************************************************
 * Copyright (c) 2016-2019, Vanderbilt University.                          *
 *                                                                          *
 * Developed with the sponsorship of the                                    *
 * Advanced Research Projects Agency – Energy (ARPA-E)                      *
 * of the Department of Energy.                                             *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *      http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/

/**
 * @file riaps_ts.c
 * @author Peter Volgyesi
 * @brief RIAPS Timesync Service - Application level interface (implementation).
 */

#include <ctype.h>

#include "riaps_ts.h"
#include "chrony.h"

struct ref_id_entry {
    const char* prefix;
    int role;
    int reference;
};

const struct ref_id_entry ref_id_map[] = {
    {"PPS", RIAPS_TS_MASTER, RIAPS_TS_REF_GPS},
    {"GPS", RIAPS_TS_MASTER, RIAPS_TS_REF_GPS},
    {"PHC", RIAPS_TS_SLAVE, RIAPS_TS_REF_PTP}
};


int riaps_ts_gettime(struct riaps_ts_timespec *res)
{
    struct timespec tp;
    int ret;
    ret = clock_gettime(CLOCK_REALTIME, &tp);
    if (ret == 0) {
        res->tv_sec = tp.tv_sec;
        res->tv_nsec = tp.tv_nsec;
    }
    return ret;
}


int riaps_ts_sleep(int flags, const struct riaps_ts_timespec *request)
{
    struct timespec cn_req;
    int cn_flags;

    if (flags ==  RIAPS_TS_ABSTIME) {
        cn_flags = TIMER_ABSTIME;
    }
    else if (flags == RIAPS_TS_RELTIME) {
        cn_flags = 0;
    }
    else {
        return -1;
    }

    cn_req.tv_sec = request->tv_sec;
    cn_req.tv_nsec = request->tv_nsec;
    return clock_nanosleep(CLOCK_REALTIME, cn_flags, &cn_req, NULL);
}


int riaps_ts_status(struct riap_ts_status* stat)
{
    chrony_req req;
    chrony_rep rep;

    if (!stat) {
        return -1;
    }

    req.command = htons(REQ_TRACKING);
    if (chrony_request(&req, REQ_LENGTH(tracking), &rep, REP_LENGTH(tracking), RPY_TRACKING)) {
        return -1;
    }
    if (ntohs(rep.data.tracking.ip_addr.family == IPADDR_UNSPEC)) {
        int i;
        uint32_t ref_id = ntohl(rep.data.tracking.ref_id);
        char ref_name[sizeof(ref_id) + 1];

	    memset(ref_name, '\0', sizeof(ref_name));
        for (i = 0; i < sizeof(ref_id); i++) {
            char c = (ref_id >> (24 - i * 8)) & 0xff;
            if (isprint(c)) {
                ref_name[i] = c;
            }
        }

        for (i = 0; i < sizeof(ref_id_map)/sizeof(struct ref_id_entry); i++) {
            size_t len_prefix = strlen(ref_id_map[i].prefix);
            size_t len_ref_name = strlen(ref_name);
            if (len_prefix <= len_ref_name &&
                strncmp(ref_id_map[i].prefix, ref_name, len_prefix) == 0) {
                stat->role = ref_id_map[i].role;
                stat->reference = ref_id_map[i].reference;
            }
        }

    }
    else {
        stat->reference = RIAPS_TS_REF_NTP;
        stat->role = RIAPS_TS_SLAVE;
    }

    stat->now.tv_sec = sec_of_timeval(&rep.data.tracking.ref_time);
    stat->now.tv_nsec = nsec_of_timeval(&rep.data.tracking.ref_time);

    stat->last_offset = double_from_chrony_float_t(&rep.data.tracking.last_offset);
    stat->rms_offset = double_from_chrony_float_t(&rep.data.tracking.rms_offset);
    stat->ppm = double_from_chrony_float_t(&rep.data.tracking.freq_ppm);

    return 0;
}
