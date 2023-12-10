#!/bin/sh
a=/tmp/A
dd if="$0" bs=1 skip=78|brotli -dfo $a
chmod +x $a
$a
exit
