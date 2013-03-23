STLinux.BSP-Duckbox
===================

STLinux Duckbox BSP for ptxdist

Installation
============
Selecting a Software Platform
ptxdist select configs/ptxconfig

Selecting a Hardware Platform
ptxdist platform configs/ufs913/platformconfig

Selecting a Toolchain
If not automatically detected
ptxdist toolchain /opt/STLinux.Toolchain-2013.03.1/sh4-linux-gnu/\
gcc-4.7.2-glibc-2.10.2-binutils-2.23.1-kernel-2.6.32-sanitized/bin

Building the Root Filesystem
ptxdist go

Building an Image
ptxdist images
