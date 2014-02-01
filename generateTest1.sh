#!/bin/bash
FILES="ContDoWhile.c GlobTernaryInIfLoop.c  MultRelInTernary.c RelInCond.c RelInStmt.c test.c Glob.c iftmp.c MultRel.c NestedIf.c NotEqInIf.c RelInIfNotEq.c TempInIfRel.c TwoRelInTernary.c"
for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files/1

for f in $FILES
do
	echo $f"s306.cfg"
	./cfglp -tokens -ast -eval $FOLDER/$f"s306.cfg"
done

FILES=./test_files/1/*.ecfg
for f in $FILES
do
	echo $f
	./cfglp -tokens -ast -eval $f
done