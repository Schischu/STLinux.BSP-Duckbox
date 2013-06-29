#!/bin/sh
LOG="logger -p user.info -t mdev-mount"
WARN="logger -p user.warn -t mdev-mount"

MOUNTBASE=/media
MOUNTPOINT="$MOUNTBASE/$MDEV"
ROOTDEV=$(readlink /dev/root)

# do not add or remove root device again...
[ "$ROOTDEV" = "$MDEV" ] && exit 0

create_symlinks() {
	DEVBASE=${MDEV:0:3} # first 3 characters
	PARTNUM=${MDEV:3}   # characters 4-
	read MODEL < /sys/block/$DEVBASE/device/model
	MODEL=${MODEL// /_} # replace ' ' with '_'
	OLDPWD=$PWD
	cd $MOUNTBASE
	if which blkid > /dev/null; then
		BLKID=$(blkid /dev/$MDEV)
		eval ${BLKID#*:}
	fi
	if [ -n "$LABEL" ]; then
		rm -f "$LABEL"
		ln -s $MDEV "$LABEL"
	fi
	if [ -n "$UUID" ]; then
		LINK="${TYPE}${TYPE:+-}${UUID}"
		rm -f "${LINK}"
		ln -s $MDEV "${LINK}"
	fi
	if [ -n "$MODEL" ]; then
		LINK="${MODEL}${PARTNUM:+-}${PARTNUM}"
		rm -f "${LINK}"
		ln -s $MDEV "${LINK}"
	fi
	BUS=""
	PORT=""
	P=$PHYSDEVPATH
	case "$P" in
	/devices/platform/stm-usb.?/stm-ehci.?/usb?/?-?/?-?.?/?-?.?:?.?/host*) # hub
		PORT=${P#*.*.}	# strip off /devices/platform/stm-usb.?/stm-ehci.?/usb?/?-?/?-?
		PORT=${PORT%%/*}	# strip off /?-?.?:?.?/host*, leaving the port
		BUS="usb-${P:31:1}-hub-${PORT}"
		;;
	/devices/platform/stm-usb.?/stm-ehci.?/usb?/?-?/?-?:?.?/host*) # no hub
		BUS="usb-${P:31:1}"
		;;
	*)
		# BUS="unknown" # ignored for now
		;;
	esac
	if [ -n "$BUS" ]; then
		LINK="${BUS}${PARTNUM:+-}${PARTNUM}"
		rm -f "${LINK}"
		ln -s $MDEV "${LINK}"
	fi

	cd $OLDPWD
}

remove_symlinks() {
	OLDPWD=$PWD
	cd $MOUNTBASE
	for i in *; do
		[ -L "$i" ] || continue
		TARGET=$(readlink "$i")
		if [ "$TARGET" = "$MDEV" ]; then
			rm "$i"
		fi
	done
	cd $OLDPWD
}

case "$ACTION" in
	add|"")
		$LOG "mounting /dev/$MDEV to $MOUNTPOINT"
		NTFSMOUNT=$(which ntfs-3g)
		RET2=$?
		# remove old mountpoint symlinks we might have for this device
		rm -f $MOUNTPOINT
		mkdir -p $MOUNTPOINT
		for i in 1 2 3 4 5; do # retry, my freeagent drive sometimes needs more time
			# $LOG "mounting /dev/$MDEV to $MOUNTPOINT try $i"
			OUT1=$(mount -t auto /dev/$MDEV $MOUNTPOINT 2>&1 >/dev/null)
			RET1=$?
			[ $RET1 = 0 ] && break
			sleep 1
		done
		if [ $RET1 != 0 -a -n "$NTFSMOUNT" ]; then
			# failed,retry with ntfs-3g
			for i in 1 2; do # retry only twice, waited already 5 seconds
				$NTFSMOUNT /dev/$MDEV $MOUNTPOINT
				RET2=$?
				[ $RET2 = 0 ] && break
				sleep 1
			done
		fi
		if [ $RET1 = 0 -o $RET2 = 0 ]; then
			create_symlinks
		else
			$WARN "mount   /dev/$MDEV $MOUNTPOINT failed with $RET1"
			$WARN "        $OUT1"
			if [ -n "$NTFSMOUNT" ]; then
				$WARN "ntfs-3g /dev/$MDEV $MOUNTPOINT failed with $RET2"
			fi
			rmdir $MOUNTPOINT
		fi
		;;
	remove)
		$LOG "unmounting /dev/$MDEV"
		grep -q "^/dev/$MDEV " /proc/mounts || exit 0 # not mounted...
		umount -lf /dev/$MDEV
		RET=$?
		if [ $RET = 0 ]; then
			rmdir $MOUNTPOINT
			remove_symlinks
		else
			$WARN "umount /dev/$MDEV failed with $RET"
		fi
		;;
esac
echo "BUS = $BUS"
echo "P = $P"