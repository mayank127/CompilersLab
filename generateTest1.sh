#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f"1" >>out1
	./cfglp -eval -ast -tokens  -d $f >>out1
	echo $f"2" >> out2
	./cfglp64 -tokens -eval -ast  -d $f >>out2
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
	echo $f"s306.cfg" >> out3
	./cfglp -eval -ast -tokens -d $FOLDER/$f"s306.cfg" >> out3
	echo $f"s306.cfg" >> out4
	./cfglp64 -tokens -eval -ast -d $FOLDER/$f"s306.cfg" >> out4
done

# meld out1 out2

#FloatVal.ecfg