STLinux.BSP-Duckbox [![Build Status](http://duckbox.de:8080/buildStatus/icon?job=STLinux.BSP-Duckbox)](http://duckbox.de:8080/job/STLinux.BSP-Duckbox/)
===================

STLinux Duckbox BSP for ptxdist

Prerequisites
=============
Install rpm2cpio, setuptdt.sh
Also install libsdl-image1.2-dev

Installation
============
wget https://raw.github.com/Schischu/STLinux.StartHere/master/start.sh; chmod 755 start.sh <br />
BOXTYPE=ufs912 SW=enigma2 MEDIAFW=gstreamer ./start.sh

Possible combinations are
SW=enigma2 MEDIAFW=gstreamer
SW=enigma2 MEDIAFW=libeplayer
SW=xbmc    MEDIAFW=gstreamer

