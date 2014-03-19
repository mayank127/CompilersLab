#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f"" >>out1
	./cfglp -icode -d $f >>out1
	echo $f"" >> out2
	./cfglp64 -icode -d  $f >>out2
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
	./cfglp -icode -d $FOLDER/$f"s306.cfg" >> out3
	echo $f"s306.cfg" >> out4
	./cfglp64 -icode  -d $FOLDER/$f"s306.cfg" >> out4
done

# meld out1 out2

#FloatVal.ecfg