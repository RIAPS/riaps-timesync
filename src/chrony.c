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


/**
 * @file chrony.c
 * @author Peter Volgyesi
 * @brief RIAPS Timesync Service - Communication with the chrony daemon (implementation).
 */


#include <math.h>

#include "chrony.h"

static int chrony_socket = -1;
static int chrony_seq = 0;

static int close_chrony_socket()
{
    close(chrony_socket);
    chrony_socket = -1;
}


static int get_chrony_socket()
{
    if (chrony_socket < 0) {
        struct hostent *chrony_host;
        struct sockaddr_in chrony_addr;
        struct timeval timeout = {CHRONY_TIMEOUT, 0};

        chrony_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (chrony_socket < 0) {
            chrony_socket = 0;
            goto ondemand_end;
        }

        if (setsockopt(chrony_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                        sizeof(struct timeval))) {
            close_chrony_socket();
            goto ondemand_end;
        }

        chrony_host = gethostbyname(CHRONY_CMD_ADDR);
        if (chrony_host == NULL) {
            close_chrony_socket();
            goto ondemand_end;
        }

        bzero((char *) &chrony_addr, sizeof(chrony_addr));
        chrony_addr.sin_family = AF_INET;
        bcopy((char *)chrony_host->h_addr,
            (char *)&chrony_addr.sin_addr.s_addr, chrony_host->h_length);
        chrony_addr.sin_port = htons(CHRONY_CMD_PORT);
        if (connect(chrony_socket, (struct sockaddr *) &chrony_addr,
                        sizeof(chrony_addr)) < 0) {
            close_chrony_socket();
            goto ondemand_end;
        }
    }

ondemand_end:
    return chrony_socket;
}

int chrony_request(chrony_req* req, int req_len, chrony_rep* rep, int rep_len, int rep_id)
{
    req->version = PROTO_VERSION_NUMBER;
    req->pkt_type = PKT_TYPE_CMD_REQUEST;
    req->res1 = 0;
    req->res2 = 0;
    req->sequence = htonl(chrony_seq++);
    req->attempt = -1;
    req->pad1 = 0;
    req->pad2 = 0;

    req_len = req_len > rep_len ? req_len : rep_len;

    while (req->attempt++ > CHRONY_MAX_RETRIES) {
        int sock_fd;
        int recvlen;

        sock_fd = get_chrony_socket();
        if (sock_fd < 0) {
            return -1;
        }
        if (send(sock_fd, (void *)req, req_len, 0) < 0) {
            close_chrony_socket();
        }

        recvlen = recv(sock_fd, (void *)rep, sizeof(chrony_rep), 0);
        if (recvlen < rep_len) {
            continue;
        }
        if (rep->sequence != req->sequence) {
            continue;
        }
        if (rep->version != PROTO_VERSION_NUMBER) {
            continue;
        }
        if (rep->pkt_type != PKT_TYPE_CMD_REPLY) {
            continue;
        }
        if (rep->command != req->command) {
            continue;
        }
        if (ntohs(rep->status) != STT_SUCCESS) {
            continue;
        }
        if (ntohs(rep->reply) != rep_id) {
            continue;
        }

        // everything seems ok
        return 0;

    }

    return -1;
}


time_t sec_of_timeval(const timeval_t* timeval)
{
    time_t sec;
    uint32_t sec_low = ntohl(timeval->tv_sec_low);
    uint32_t sec_high = ntohl(timeval->tv_sec_high);

    if (sec_high == TV_NOHIGHSEC) {
        sec_high = 0;
    }

    if (sizeof(sec) > sizeof(sec_low) && sec_high != TV_NOHIGHSEC) {
        sec = (uint64_t)sec_high << 32 | sec_low;
    }
    else {
        sec = sec_low;
    }

    return sec;
}


long nsec_of_timeval(const timeval_t* timeval)
{
    return ntohl(timeval->tv_nsec);
}

#define FLOAT_EXP_BITS 7
#define FLOAT_EXP_MIN (-(1 << (FLOAT_EXP_BITS - 1)))
#define FLOAT_EXP_MAX (-FLOAT_EXP_MIN - 1)
#define FLOAT_COEF_BITS ((int)sizeof (int32_t) * 8 - FLOAT_EXP_BITS)
#define FLOAT_COEF_MIN (-(1 << (FLOAT_COEF_BITS - 1)))
#define FLOAT_COEF_MAX (-FLOAT_COEF_MIN - 1)

double double_from_chrony_float_t(const chrony_float_t* f)
{
    int32_t exp, coef;
    uint32_t x;

    x = ntohl(f->f);

    exp = x >> FLOAT_COEF_BITS;
    if (exp >= 1 << (FLOAT_EXP_BITS - 1))
        exp -= 1 << FLOAT_EXP_BITS;
    exp -= FLOAT_COEF_BITS;

    coef = x % (1U << FLOAT_COEF_BITS);
    if (coef >= 1 << (FLOAT_COEF_BITS - 1))
        coef -= 1 << FLOAT_COEF_BITS;

    return coef * pow(2.0, exp);
}