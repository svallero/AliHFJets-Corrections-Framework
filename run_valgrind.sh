#!/bin/bash
rm valgrind.log
valgrind="/home/svallero/valgrind/bin/valgrind"
opt="--tool=memcheck"
opt="${opt} --leak-check=full" 
opt="${opt} --show-leak-kinds=definite" 
#opt="${opt} --show-leak-kinds=indirect" 
#opt="${opt} --show-leak-kinds=possible" 
#opt="${opt} --show-leak-kinds=all" 
#opt="${opt} --show-reachable=yes" 
opt="${opt} --log-file=valgrind.log" 
opt="${opt} --track-origins=yes" 
#opt="${opt} --dsymutil=yes" 
opt="${opt} --gen-suppressions=all" 
opt="${opt} --suppressions=./suppressions_standard_vaf.supp" 
#opt="${opt} -v" 

export executable="RunVaf.C" 
echo "Running *** $executable *** on Valgrind"
echo with options: $opt
$valgrind $opt root.exe -b -q -x $executable 
