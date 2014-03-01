#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f >>out1
	echo $f >> out2
	./cfglp -tokens -d $f >>out1
	./cfglp64 -tokens -d $f >>out2
done

FILES="ArithRelFn.c       EmptyBB.c         fun_arg_arth_rel.c   mul_fun.c         ParamActualSame.c  recursivecall.c  VoidFnPassParam.c \
ArithRelInOutFn.c  EmptyFn.c         Glob.c               MultFn.c          PassParam.c        return_float.c   void_function.c \
CatchFloatInInt.c  Fact.c            GlobInMid.c          NoFnCall.c        PassParamDiff.c    RetVal.c         VoidRet.c \
CatchRet.c         fn_call_return.c  GlobPassSameParam.c  OperationParam.c  PassParamRet.c     t2.c \
DTemp.c            FnPrtyp.c         GlobUse.c            OperationRet.c    recursion.c        test.c \
"

for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files

for f in $FILES
do
	echo $f"s306.cfg" >>  out1
	echo $f"s306.cfg" >> out2
	./cfglp -tokens -d $FOLDER/$f"s306.cfg" >>out1
	./cfglp64 -tokens -d $FOLDER/$f"s306.cfg" >>out2
done

# meld out1 out2

#FloatVal.ecfg