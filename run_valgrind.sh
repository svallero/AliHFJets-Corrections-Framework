#!/bin/bash
rm valgrind.log
valgrind="valgrind"
opt="--tool=memcheck"
opt="${opt} --leak-check=full" 
opt="${opt} --show-leak-kinds=definite" 
#opt="${opt} --show-leak-kinds=indirect" 
#opt="${opt} --show-leak-kinds=possible" 
#opt="${opt} --show-leak-kinds=all" 
#opt="${opt} --show-reachable=yes" 
opt="${opt} --log-file=valgrind.log" 
opt="${opt} --track-origins=yes" 
opt="${opt} --dsymutil=yes" 
opt="${opt} --gen-suppressions=all" 
#opt="${opt} --suppressions=./suppressions.supp" 
opt="${opt} --suppressions=./suppressions_standard.supp" 
#opt="${opt} --suppressions=/opt/alice/root/v5-34-11/etc/valgrind-root.supp" 
#opt="${opt} -v" 

export executable="RunSara.C" 
#export executable="test.C" 
echo "Running *** $executable *** on Valgrind"
echo with options: $opt
$valgrind $opt root.exe -b -q -x $executable 
#$valgrind $opt root.exe -b -q  
