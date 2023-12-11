#!/bin/sh
a=/tmp/A
tail -c+66 "$0"|brotli -d>$a
chmod +x $a
$a
>
