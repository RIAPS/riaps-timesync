"""
RIAPS Timesync Service
======================

Copyright (C) Vanderbilt University, ISIS 2016
"""
import os
import ctypes
import numbers

lib_riaps_ts = ctypes.CDLL("libriaps_ts.so")

RIAPS_TS_RELTIME = 0
RIAPS_TS_ABSTIME = 1
RIAPS_TS_MASTER = 0
RIAPS_TS_SLAVE = 1
RIAPS_TS_REF_NONE = 0
RIAPS_TS_REF_GPS = 1
RIAPS_TS_REF_NTP = 2
RIAPS_TS_REF_PTP = 3

class riaps_ts_timespec(ctypes.Structure):
    _fields_ = [
        ('tv_sec', ctypes.c_long),
        ('tv_nsec', ctypes.c_long)
    ]

class riaps_ts_status(ctypes.Structure):
    _fields_ = [
        ('role', ctypes.c_int),
        ('reference', ctypes.c_int),
        ('now', riaps_ts_timespec),
        ('last_offset', ctypes.c_double),
        ('rms_offset', ctypes.c_double),
        ('ppm', ctypes.c_double)
    ]

_riaps_ts_gettime = lib_riaps_ts.riaps_ts_gettime
_riaps_ts_gettime.argtypes = [ctypes.POINTER(riaps_ts_timespec)]

def gettime():
    """Get the current system / synchornized time.

    The time information will be returned in a (sec, nsec) integer tuple.
    """
    res = riaps_ts_timespec()
    if _riaps_ts_gettime(ctypes.pointer(res)):
        errno_ = ctypes.get_errno()
        raise OSError(errno_, os.strerror(errno_))
    return (res.tv_sec, res.tv_nsec)

_riaps_ts_sleep = lib_riaps_ts.riaps_ts_sleep
_riaps_ts_sleep.argtypes = [ctypes.c_int, ctypes.POINTER(riaps_ts_timespec)]

def sleep(timespec, flags=RIAPS_TS_RELTIME):
    """Sleep for the specified amount of time.

    flags is either RIAPS_TS_RELTIME or RIAPS_TS_ABSTIME.
    timespec is a (sec, nsec) integer sequence or single number (sec)
    """
    if isinstance(timespec, numbers.Number):
        tv_sec = int(timespec)
        tv_nsec = int(1e9 * (timespec - tv_sec))
        timespec = (tv_sec, tv_nsec)
    request = riaps_ts_timespec(*timespec)
    if _riaps_ts_sleep(flags, ctypes.pointer(request)):
        errno_ = ctypes.get_errno()
        raise OSError(errno_, os.strerror(errno_))


_riaps_ts_status = lib_riaps_ts.riaps_ts_status
_riaps_ts_status.argtypes = [ctypes.POINTER(riaps_ts_status)]
def status():
    """Get the current status of the time synchonization service.

    The result is a tuple with the following elements:
    (role, reference, ref_time, last_offset, rms_offset, ppm)
    """
    stat = riaps_ts_status()
    if _riaps_ts_status(ctypes.pointer(stat)):
        errno_ = ctypes.get_errno()
        raise OSError(errno_, os.strerror(errno_))

    return (stat.role,
            stat.reference,
            (stat.now.tv_sec, stat.now.tv_nsec),
            stat.last_offset,
            stat.rms_offset,
            stat.ppm)


