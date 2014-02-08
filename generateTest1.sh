#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f >> out1
	echo $f >> out2
	./cfglp -tokens $f -d >> out1
	./cfglp64 -tokens   -d $f >> out2
done

FILES="Arith.c          DeclDef1BB.c      GlobNoDeclNoDef1BB.c   IfIntFloatRelArith.c  MultGlob.c          NotEqInIf.c     test.c \
ArithFloat.c     FloatRelArith.c   GlobTernaryInIfLoop.c  IfRelArith.c          MultRel.c           RelArith.c      TwoRelInTernary.c \
ArithIntFloat.c  FloatUnary.c      IfArith.c              iftmp.c               MultRelInTernary.c  RelInCond.c     Unary.c \
Const.c          Glob.c            IfFloatArith.c         IfUnary.c             NestedIf.c          RelInIfNotEq.c  UserIfTmp.c \
ContDoWhile.c    GlobDeclDef.c     IfFloatRelArith.c      RelInStmt.c           VarMain.c \
Decl1BB.c        GlobLocSameVar.c  IfIntFloat.c           IntInFloat.c          NoDeclNoDef1BB.c    TempInIfRel.c"

for f in $FILES
do
	echo $f
	# make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files

for f in $FILES
do
	echo $f"s306.cfg" >> out1
	echo $f"s306.cfg" >> out2
	# ./cfglp -tokens  $FOLDER/$f"s306.cfg" >> out1
	# ./cfglp64 -tokens $FOLDER/$f"s306.cfg" >> out2
done

# meld out1 out2