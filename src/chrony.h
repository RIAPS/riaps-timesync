/****************************************************************************
 * Copyright (c) 2016-2024, Vanderbilt University.                          *
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
#ifndef _CHRONY_H_
#define _CHRONY_H_


/**
 * @file chrony.h
 * @author Peter Volgyesi
 *
 * @brief RIAPS Timesync Service - Communication with the chrony daemon.
 *
 * This module contains a lightweight interface to communicate with the
 * running chrony daemon via its UDP command socket. The functions in this
 * module are not supposed to be used by application level code, but provided
 * for higher-level interfaces in the riaps-timesync service.
 */


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CHRONY_CMD_ADDR "127.0.0.1" /**< Only local chrony daemons are supported */
#define CHRONY_CMD_PORT 323         /**< Hardwired command (UDP) port of chrony */

#define MAX_PADDING_LENGTH 396      /**< Hardwired chrony communication parameter */
#define PROTO_VERSION_NUMBER 6      /**< Currently supported CMD protocol */

#define CHRONY_TIMEOUT 1            /**< Connection timeout in seconds */
#define CHRONY_MAX_RETRIES 3        /**< Number of connection retries before giving up */

/*** Custom types ***/
#define IPADDR_UNSPEC 0
#define IPADDR_INET4 1
#define IPADDR_INET6 2

/**
 * @brief Chrony's 32-bit floating point type. @see double_from_chrony_float_t()
 */
typedef struct
{
    int32_t f;
} chrony_float_t;

/**
 * @brief Definition of @c timeval_t using 32-bit integers
 */
typedef struct
{
    uint32_t tv_sec_high;
    uint32_t tv_sec_low;
    uint32_t tv_nsec;
} timeval_t;
#define TV_NOHIGHSEC 0x7fffffff

typedef struct
{
    union {
        uint32_t in4;
        uint8_t in6[16];
    } addr;
    uint16_t family;
} ipaddr_t;

#define PKT_TYPE_CMD_REQUEST 1
#define PKT_TYPE_CMD_REPLY 2

/*** Requests ***/
#define REQ_TRACKING 33

/**
 * @brief Chrony end-of-request (null request)
 */
typedef struct
{
    int32_t EOR;
} req_null;


/**
 * @brief Chrony CMD protocol request datagram format
 */
typedef struct
{
    uint8_t version;
    uint8_t pkt_type;
    uint8_t res1;
    uint8_t res2;
    uint16_t command;
    uint16_t attempt;
    uint32_t sequence;
    uint32_t pad1;
    uint32_t pad2;
    union {
        req_null null;
        req_null tracking;
    } data;
    uint8_t padding[MAX_PADDING_LENGTH];
} chrony_req;

/*** Replies ***/

#define STT_SUCCESS 0

#define RPY_TRACKING 5

typedef struct
{
    int32_t EOR;
} rep_null;

/**
 * @brief Chrony CMD protocol tracking datagram format
 */
typedef struct
{
    uint32_t ref_id;
    ipaddr_t ip_addr;
    uint16_t stratum;
    uint16_t leap_status;
    timeval_t ref_time;
    chrony_float_t current_correction;
    chrony_float_t last_offset;
    chrony_float_t rms_offset;
    chrony_float_t freq_ppm;
    chrony_float_t resid_freq_ppm;
    chrony_float_t skew_ppm;
    chrony_float_t root_delay;
    chrony_float_t root_dispersion;
    chrony_float_t last_update_interval;
    int32_t EOR;
} rep_tracking;

/**
 * @brief Chrony CMD protocol reply datagram format
 */
typedef struct
{
    uint8_t version;
    uint8_t pkt_type;
    uint8_t res1;
    uint8_t res2;
    uint16_t command;
    uint16_t reply;
    uint16_t status;
    uint16_t pad1;
    uint16_t pad2;
    uint16_t pad3;
    uint32_t sequence;
    uint32_t pad4;
    uint32_t pad5;

    union {
        rep_null null;
        rep_tracking tracking;
    } data;

} chrony_rep;

#define REQ_LENGTH(reply_data_field) \
  offsetof(chrony_req, data.reply_data_field.EOR)

#define REP_LENGTH(reply_data_field) \
  offsetof(chrony_rep, data.reply_data_field.EOR)

/**
 * @brief Makes a standalone request and waits for the reply from chrony.
 *
 * Note, that both communication buffers (req/rep) have to be preallocated.
 *
 * @param req Pointer to the allocated and initialized request buffer
 * @param req_len The size of the allocated request buffer
 * @param rep Pointer to the pre-allocated reply buffer
 * @param rep_len The size of the pre-allocated reply buffer
 * @param rep_id Filtering response messages to match this ID
 * @return Zero, if succeeded and a valid response has been received.
 */
int chrony_request(chrony_req* req, int req_len, chrony_rep* rep, int rep_len, int rep_id);

/**
 * @brief Get the integer (seconds) part of the @c timeval_t value
 *
 * @param timeval Pointer to the @c timeval_t value
 * @return The integer part in seconds
 */
time_t sec_of_timeval(const timeval_t* timeval);

/**
 * @brief Get the fractional (nanosec) part of the @c timeval_t value
 *
 * @param timeval Pointer to the @c timeval_t value
 * @return The fractional part in integer nanoseconds
 */
long nsec_of_timeval(const timeval_t* timeval);

/**
 * @brief Convert from chrony 32-bit floating point type to double. 
 *
 * @param f Pointer to the 32-bit source value
 * @return The standard C double representation of the floating point value
 */
double double_from_chrony_float_t(const chrony_float_t* f);

#endif // _CHRONY_H_