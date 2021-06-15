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
#ifndef _RIAPS_TS_H_
#define _RIAPS_TS_H_


/**
 * @file riaps_ts.h
 * @author Peter Volgyesi
 *
 * @brief RIAPS Timesync Service - Application level interface.
 *
 * This module contains a simple interface for monitoring the status of the time
 * synchronization service and for getting the current synchronized time and waiting
 * on/for a synchronized time instant.
 */

#include <time.h>

#define RIAPS_TS_RELTIME 0 /**< Wait for relative time interfval (flag) @see riaps_ts_sleep() */
#define RIAPS_TS_ABSTIME 1 /**< Wait for absolute time instant (flag) @see riaps_ts_sleep() */
#define RIAPS_TS_MASTER 0  /**< Master role value @see riap_ts_status */
#define RIAPS_TS_SLAVE 1   /**< Slave role value @see riap_ts_status */
#define RIAPS_TS_REF_NONE 0  /**< No external reference clock is used/valid @see riap_ts_status */
#define RIAPS_TS_REF_GPS 1   /**< GPS is used as timing reference @see riap_ts_status*/
#define RIAPS_TS_REF_NTP 2   /**< An external NTP server is used as timing reference @see riap_ts_status */
#define RIAPS_TS_REF_PTP 3   /**< A PTP server is the current timing reference @see riap_ts_status */

struct riaps_ts_timespec {
    long    tv_sec;        /* seconds, always 32 bit */
    long    tv_nsec;       /* nanoseconds */
};

/**
 * @brief Current status description of the timesync service
 */
struct riap_ts_status {
    int role;               /**< Synchronization role of this node */
    int reference;          /**< The current/valid timing reference clock for this node */
    struct riaps_ts_timespec now; /**< Current synchronized time (unless RIAPS_TS_REF_NONE) */
    double last_offset;     /**< Last error value from the current reference */
    double rms_offset;      /**< Filtered, long-term RMS timesync error (quaility of timesync) */
    double ppm;                /**< The local (compensated) time drift in parts per million */
};

/**
 * @brief Wrapper function to query the proper (synchronized) local time 
 *
 * @param res Pointer to a pre-allocated @c riaps_ts_timespec structure to be filled in.
 * @return Zero, if succeeded and a valid time value is provided.
 */
int riaps_ts_gettime(struct riaps_ts_timespec *res);

/**
 * @brief Wrapper function to wait for the proper (synchronized) local time.
 *
 * Can wait for relative time intervals or for an absolute time instant.
 *
 * @param flags Relative or absolute wait. @see RIAPS_TS_RELTIME, RIAPS_TS_ABSTIME
 * @return Zero, if successfully sleeping for the requested interval.
 */
int riaps_ts_sleep(int flags, const struct riaps_ts_timespec *request);

/**
 * @brief Query the current status of the time synchronization service.
 *
 * @param stat Pre-allocated buffer to receive the status information. @see riap_ts_status
 * @return Zero, if succeeded and valid status is provided.
 */
int riaps_ts_status(struct riap_ts_status* stat);

#endif // _RIAPS_TS_H_
