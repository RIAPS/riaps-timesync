#!/usr/bin/env python3
"""Simple test for the RIAPS Timesync Service."""
import riaps_ts

def main():
    while True:
        now = riaps_ts.gettime()
        print('wake: %d.%09d secs' % now)
        # status = riaps_ts.status()
        # print(('\trole: %d\n' +
        #        '\treference: %d\n' +
        #        '\tnow: %d.%09d secs\n' +
        #        '\tlast_offset: %.9f secs\n' +
        #        '\trms_offset: %.9f secs\n' +
        #        '\tppm: %.6f') % status)

        # Try to wake up on second boundaries (PPS)
        snooze = (now[0] + 1, 0)
        riaps_ts.sleep(snooze, riaps_ts.RIAPS_TS_ABSTIME)

if __name__ == '__main__':
    main()
