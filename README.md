# HomeKit for Bluetooth (BLE)

**This project has moved to https://gitlab.com/timwilkinson/HomeKit**

Apple's HomeKit protocol supports both IP and BLE devices. While there appear to be a few open source implementations
of IP stacks around (notably https://github.com/KhaosT/HAP-NodeJS), I couldn't find any BLE stacks. So, here's one for
you to play with.

This stack runs on the Nordic BLE nRF51 series of chips. The projects compiles using the "standard" Eclipse/GCC toolchain
setup.

# NOTE

**This code hasn't been updated in a long time, and it's been drawn to my attention that Apple's newer BLE spec for HomeKit is vastly different to this version.**

# Code

The code provides all the services required to pair iOS with a BLE device and to operate that device once paired. It
runs on the Nordic nRF51 PCA10028 development board.

## Timings

Here are some perliminary timings. Note that most of the crypto code is in C but with some assembly used to speed the 256-bit integer multiplies. Also note that these timings do not include the time to send and receive payloads. But, anyway, numbers:

### nRF51

#### Pairing

Pairing is dominated by the SRP algorithm which is very slow and expensive. Fortunately this only happens once when the iOS device is being associated with the HomeKit device:

Time: 40 seconds

#### Verify

Verify happens everytime an iOS device reconnected to the HomeKit device. Ideally this should be as fast as possible. I've included the C-only and assembly timings here - the difference is dramatic:

Time (C code): 4 seconds

Time (with assembly multiply): 1.2 seconds

### nRF52

Recently Nordic released their next generation BLE platform. I though it might be interesting to see how well it performs Pairing and Verify. As you can see, this chip is much, much faster:

#### Pairing

Time: 3.7 seconds

#### Verify

Time (with assembly multiply): 0.26 seconds

## Memory

The HomeKit code is approximately 28K (compiled with -Os) and uses 1K of RAM. During Pairing an additional 11K of stack RAM is used.

# Thanks

I want to thank a number of projects which made this possible:

1. https://github.com/KhaosT/HAP-NodeJS - which documents the HomeKit protocols for IP and allowed me to guess how they
were implemented over Bluetooth.

2. http://tweetnacl.cr.yp.to - which provides the compact eliptical curve implementations, as well as the sha512 hash.

3. https://tls.mbed.org - which provides the core multi-precission math routines used in the SRP implementation.

4. http://munacl.cryptojedi.org/ - which provides the ARM Cortex-M0 optimized Curve25519 implementation and fast multiply routines.

# Notes

Please note that this software was produced without any reference to any propriatery documentation or information. I
am not a MFi licensee, nor do I have access to any related information.
