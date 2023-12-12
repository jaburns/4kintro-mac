#!/bin/sh
a=/tmp/A
tail -c+74 "$BASH_SOURCE"|gzip -d>$a
chmod +x $a
$a
>
