#!/usr/bin/bash

cd $2

mkdir -p .cmp

for f in *.o ;
do
    $1 -p --strip-debug $f .cmp/${f/.o/.cmp.o}
done
