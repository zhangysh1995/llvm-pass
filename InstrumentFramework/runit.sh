#!/bin/bash
## run pp-check crash test

# args
if [ $# -eq 0 ] ; then
	echo "Usage: ./runit.sh path"
	echo "Done."
	exit
fi

# results="Name\t Original\t New\t\n"
regex='[0-9]*.[0-9]*'

#===== Transforms =====
# flags="clang++ -Xclang -load -Xclang ./libLLVMtest.so"

flags="opt -disable-opt -stats -time-passes -load ./libLLVMtest.so -calltrace "

#===== Run pass =====
function doit {
    local full=$?
	local retVal=$?
    local name=$?

    for f in `ls ${d}/*.bc` ;
    do
        name=`basename ${f} .bc`

        echo "Testing file: " ${name}
        full="${flags} ${f} > ${name}1.bc"
        retVal= eval $full

        size=($(wc -c < ${f}))
        sizeNew=($(wc -c < ${name}1.bc))

        echo print ${sizeNew}/${size}.  | python

        # results="${results}\t ${size}\t ${sizeNew}\t\n"

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


echo "Finished testing, exit."
exit
