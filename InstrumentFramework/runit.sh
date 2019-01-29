#!/bin/bash
## run pp-check crash test

# args
if [ $# -eq 0 ] ; then
	echo "Usage: ./runit.sh path"
	echo "Done."
	exit
fi

#===== Transforms =====
# flags="clang++ -Xclang -load -Xclang ./libLLVMtest.so"

flags="opt -load ./libLLVMtest.so -calltrace < "

#===== Run pass =====
function doit {
    for f in `ls ${d}/*.bc` ;
    do
	echo "Testing file: " $f
        local full=$?
	local retVal=$?

        full="${flags} ${f} > /dev/null"
        retVal= eval $full

        if [ $retVal -ne 0 ]; then
	    echo "There is a problem. Aborted."
            exit
        fi
    done       		
}


# list all folders
for d in `ls -d ${1}/*`;
do
    echo "===== In folder" $d
    doit $d
done

rm ${1}/*/test.bc

echo "Finished testing, exit."
exit
