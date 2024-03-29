# riaps-timesync

Time synchronization service, instrumentation and tests for the RIAPS platform.

[![Build Status](https://travis-ci.com/RIAPS/riaps-timesync.svg?token=QQcruvP29rqE9b8AjB5C&branch=master)](https://travis-ci.com/RIAPS/riaps-timesync)

## Time synchronization configuration tool: timesyncctl

The release (.deb) packages contain a configuration tool, called **timesyncctl** for an easy, semi-automated way for configuring a node for distributed time synchronization.
The tool configures and checks the status of the ChronoCape board (if needed) in the device tree and controls all services (`GPSd, chrony, phc2sys, ptp4l`, etc) needed
in a specific role.

Notes:

 - The tool is installed under `/usr/local/bin` (starting in v1.1.17, previous version were in `/opt/riaps/*<arch>*/bin`, where `*<arch>*` is either `armhf` or `amd64`).

 - Some operations (`config` and `restart`) needs root privileges, thus you need to run the tool as *root* or with the `sudo` command

The following time synchronization roles are supported:

 - **master**: the node is synchronized to (1) GPS (using the ChronoCape board) or (2) NTP (well-known internet server) or (3) NTP (the control/VM host) in this order. Selecting the best reference clock is automatic. It also acts as a PTP master on the local network.

 - **standalone**: same as master, but PTP master role is disabled.

 - **slave**: the node is forced to use PTP in slave mode. Needs a PTP master on the local network.

Running **timesyncctl**:

 - `timesyncctl config`: will list the available roles.
 - `timesyncctl config <role>`: will change the node's configuration for the given role and will restart the services. Needs root privileges.
 - `timesyncctl restart`: will restart the services in the current role. Needs root privileges.
 - `timesyncctl status`: check the status of the services and device tree in the current role.

Examples:

To configure the node for a master role:

    sudo timesyncctl config master

To check if everything is as expected in the current role:

    timesyncctl status

> Note: This package is currently optimized for the BBB Remote nodes (which use all available modes).  The RIAPS VM (amd64 package) is intended to stay in `standalone` mode since we are using a virtual machine and hardware timestamping is not available.  The Raspberry Pi 3/4 and Jetson Nano do not support hardware timestamping, so only the `standalone` mode is also used for arm64 packages here.

### Check Node Synchronization

An easy way to test that time is synchronized across nodes is to do the following on each node (including the VM):

```
git clone https://github.com/RIAPS/riaps-timesync.git
cd riaps-timesync/python
python3 test-timesync.py
```

This will provide time from riaps_timesync (riaps_ts) continuously until you use `Ctrl C` to stop it.  You can see that each of the nodes are sync’ed by comparing the output from this test.  Here is an example of the test output:

```
wake: 1637275707.000529669 secs
wake: 1637275708.000811827 secs
wake: 1637275709.000351160 secs
```

To do these commands using riaps_fab, use the following putting in the above command:

```riaps_fab sys.run:’”<command>”’```

>Note: to stop the program you need to do `Ctrl C` on the node or use `riaps_fab sys.sudo:’”pkill -f test_timesync.py”’`
