#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f >> out1
	echo $f >> out2
	./cfglp -tokens -ast -eval -d $f >> out1
	./cfglp64 -tokens -ast -eval  -d $f >> out2
done

FILES="ArithBrckt.c     Decl1BB.c        GlobLocSameVar.c       IfIntFloat.c          MultGlob.c          RelArithBrckt.c  test.c \
Arith.c          DeclDef1BB.c     GlobNoDeclNoDef1BB.c   IfIntFloatRelArith.c  MultRel.c           RelArith.c       TwoRelInTernary.c \
ArithFloat.c     EmptyIf.c        GlobRemov.c            IfRelArith.c          MultRelInTernary.c  RelInCond.c      TypeCast.c \
ArithIntFloat.c  FloatRelArith.c  iftmp.c               NestedIf.c             Unary.c \
Brckt.c          FloatUnary.c     IfArith.c              IfUnary.c                     RelInStmt.c      UnaryPlus.c \
Const.c          Glob.c           IfFloatArith.c                 NoDeclNoDef1BB.c    RetInFor.c       UserIfTmp.c \
ContDoWhile.c    GlobDeclDef.c    IfFloatRelArith.c      IntInFloat.c          NotEqInIf.c         TempInIfRel.c    VarMain.c"

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
	./cfglp -tokens -ast -eval  -d $FOLDER/$f"s306.cfg" >> out1
	./cfglp64 -tokens -ast -eval -d $FOLDER/$f"s306.cfg" >>out2
done

# meld out1 out2

#FloatVal.ecfg