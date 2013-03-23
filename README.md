STLinux.BSP-Duckbox
===================

STLinux Duckbox BSP for ptxdist

Installation
============
Selecting a Software Platform<br />
ptxdist select configs/ptxconfig

Selecting a Hardware Platform<br />
ptxdist platform configs/ufs913/platformconfig

Selecting a Toolchain<br />
If not automatically detected<br />
ptxdist toolchain /opt/STLinux.Toolchain-2013.03.1/sh4-linux-gnu/\ <br />
gcc-4.7.2-glibc-2.10.2-binutils-2.23.1-kernel-2.6.32-sanitized/bin

Building the Root Filesystem<br />
ptxdist go

Building an Image<br />
ptxdist images
