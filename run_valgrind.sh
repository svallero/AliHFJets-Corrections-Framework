#!/bin/bash
rm valgrind.log
#valgrind="/usr/local/Cellar/valgrind/3.8.1/bin/valgrind"
valgrind="valgrind"
opt="--tool=memcheck"
#opt="--tool=callgrind"
export opt="--leak-check=yes" 
#opt="${opt} --show-reachable=yes" 
opt="${opt} --log-file=valgrind.log" 
opt="${opt} --track-origins=yes" 
opt="${opt} --dsymutil=yes" 
opt="${opt} --gen-suppressions=all" 
#opt="${opt} --suppressions=/Users/svallero/ALICE/bjetcode-oct14/suppressions.supp" 
#opt="${opt} -v" 

#export executable="RunSara.C" 
#export executable="LoadLibsSara.C ReadResults.C" 
export executable="LoadLibsSara.C" 
#export executable="RunTest.C" 
echo "Running *** $executable *** on Valgrind"
echo with options: $opt
$valgrind $opt root -b -q -x $executable
