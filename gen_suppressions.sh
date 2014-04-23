#!/bin/sh

infile=$1
outfile="suppressions_standard_vaf.supp"
#rm $outfile

issupp=0
count=0
suppname="jets_possible_"
while read line; do
	if [ "$line" = "{" ]; then
                issupp=1
                echo "Generating suppression: $count"
                count=$((count+1)) 
		echo "{" >> $outfile
                echo "$suppname$count" >> $outfile

	elif [ "$line" = "}" ]; then
		echo "}" >> $outfile
                issupp=0
              elif [[ ($issupp -eq 1) && ("$line" != "<insert_a_suppression_name_here>") ]]; then
                echo "$line" >> $outfile
           
        fi  

done < $infile 
