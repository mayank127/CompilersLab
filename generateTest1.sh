#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f >>out1
	echo $f >>out2
	./cfglp -tokens -ast -eval -d $f >> out1
	./cfglp64 -tokens -ast -eval -d $f >> out2
done

FILES="ContDoWhile.c  Glob.c            GlobNoDeclNoDef1BB.c   MultRelInTernary.c  NoDeclNoDef1BB.c  RelInIfNotEq.c  test.c \
Decl1BB.c      GlobDeclDef.c     GlobTernaryInIfLoop.c  MultGlob.c      NestedIf.c          NotEqInIf.c       RelInStmt.c     TwoRelInTernary.c \
DeclDef1BB.c   GlobLocSameVar.c  iftmp.c                MultRel.c         RelInCond.c       TempInIfRel.c   VarMain.c"

for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files

for f in $FILES
do
	echo $f"s306.cfg" >> out1
	echo $f"s306.cfg" >> out2
	./cfglp -tokens -ast -eval -d $FOLDER/$f"s306.cfg" >> out1
	./cfglp64 -tokens -ast -eval -d $FOLDER/$f"s306.cfg" >> out2
done