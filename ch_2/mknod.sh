MODULE1="ch2_mod1_dev"
MAJOR1=$(awk "\$2==\"$MODULE1\" {print \$1}" /proc/devices)

MODULE2="ch2_mod2_dev"
MAJOR2=$(awk "\$2==\"$MODULE2\" {print \$1}" /proc/devices)

mknod /dev/$MODULE1 c $MAJOR1 0
mknod /dev/$MODULE2 c $MAJOR2 0
