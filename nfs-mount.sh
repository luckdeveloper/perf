#!/bin/bash
# usage: nfs-mount.sh <>
#

WANTED_COUNT=1000
MOUNT_ROOT="/mnt/nfs_mount/"
SERVER_IP="100.2.87.55"
SERVER_EXPORT="/fs/fs01/mmqq"

UMOUNT_CMD="umount"

echo $MOUNT_ROOT
# creat 1000 directory
mount_count=0
moung_path=""
mount_string=""
umount_string=""

while (($mount_count < $WANTED_COUNT))
do
    mount_path=$MOUNT_ROOT"dir"$mount_count
    eval mkdir -p $mount_path
    mount_string="mount -t nfs "$SERVER_IP":"$SERVER_EXPORT" "$mount_path
    umount_string="umount "$mount_path
    if [ "$1" = $UMOUNT_CMD ]
    then
	echo "$umount_string"
	eval $umount_string
    else
	echo "$mount_string"
	eval $mount_string
    fi

    mount_count=$(($mount_count + 1))
done

# mount 1000 directory
