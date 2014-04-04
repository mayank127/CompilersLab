#!/bin/bash
if [ "$1" -eq 0 ]; then
	echo "" > out1
elif [ "$1" -eq 1 ]; then
	echo "" > out2
fi
EFILES=./test_files/*.ecfg
TOKENS="-icode -compile -lra -symtab"
ETOKENS="-icode -compile -lra -symtab -d"

cd test_files/
make clean
cd ../
for f in $EFILES
do
	if [ "$1" -eq 0 ]; then
		echo $f"" >>out1
		./cfglp $ETOKENS  $f >>out1
	elif [ "$1" -eq 1 ]; then
		echo $f"" >> out2
		./cfglp64 $ETOKENS $f >>out2
	fi
done

cd test_files/; FILES=$(ls *.c); cd ..;

for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files
echo "" > out3
echo "" > out4
for f in $FILES
do
	if [ "$1" -eq 0 ]; then
		echo $f"s306.cfg"
		./cfglp $TOKENS $FOLDER/$f"s306.cfg"
	elif [ "$1" -eq 1 ]; then
		echo $f"s306.cfg"
		./cfglp64 $TOKENS $FOLDER/$f"s306.cfg"
		rm -fr ../test_files
		mkdir ../test_files
		cp test_files/* ../test_files/
	fi
done

# meld out1 out2

#FloatVal.ecfg

diff -r ./test_files ../test_files > out
diff out1 out2 >>out