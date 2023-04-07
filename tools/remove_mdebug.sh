#!/usr/bin/bash

cd $1

if [[ ! -d ".mdebug/" ]]
then
    mkdir .mdebug
fi

for f in $(find . -name '*.o' -not -path './.mdebug/*') ;
do
    cp $f .mdebug/$f
    mips-linux-gnu-objcopy --remove-section .mdebug $f
done


