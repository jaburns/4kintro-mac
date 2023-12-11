#!/bin/sh
a=/tmp/A
tail -c+76 "$BASH_SOURCE"|brotli -d>$a
chmod +x $a
$a
>
