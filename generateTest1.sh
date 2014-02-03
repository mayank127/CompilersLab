#!/bin/bash
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
	echo $f"s306.cfg"
	./cfglp -tokens -ast -eval $FOLDER/$f"s306.cfg"
done

FILES=./test_files/*.ecfg
for f in $FILES
do
	echo $f
	./cfglp -tokens -ast -eval $f
done