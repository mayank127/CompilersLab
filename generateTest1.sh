#!/bin/bash

echo "" > out1
echo "" > out2
EFILES=./test_files/*.ecfg
for f in $EFILES
do
	echo $f >>out1
	echo $f >> out2
	./cfglp -eval -ast -tokens   $f >>out1
	# ./cfglp64 -tokens -eval -ast  $f >>out2
done

FILES="ArithBrckt.c       Decl1BB.c         fun_arg_arth_rel.c    IfFloatRelArith.c     MultRelInTernary.c  PassParamRet.c   TempInIfRel.c \
Arith.c            DeclDef1BB.c      Glob.c                IfIntFloat.c          NestedIf.c          recursion.c      test.c \
ArithFloat.c       DTemp.c           GlobDeclDef.c         IfIntFloatRelArith.c  recursivecall.c  TwoRelInTernary.c \
ArithIntFloat.c    EmptyBB.c         GlobInMid.c           IfRelArith.c          NoDeclNoDef1BB.c    RelArithBrckt.c  TypeCast.c \
ArithRelFn.c       EmptyFn.c         GlobLocSameVar.c      iftmp.c               NoFnCall.c          RelArith.c       Unary.c \
ArithRelInOutFn.c  EmptyIf.c         GlobNoDeclNoDef1BB.c  IfUnary.c             NotEqInIf.c         RelInCond.c      UnaryPlus.c \
Brckt.c            Fact.c            GlobPassSameParam.c   IntInFloat.c          OperationParam.c    RelInStmt.c      UserIfTmp.c \
CatchFloatInInt.c  FloatRelArith.c   GlobRemov.c           mul_fun.c             OperationRet.c      RetInFor.c       VarMain.c \
CatchRet.c         FloatUnary.c      GlobUse.c             MultFn.c              ParamActualSame.c   return_float.c   VoidFnPassParam.c \
Const.c            fn_call_return.c  IfArith.c             MultGlob.c            PassParam.c         RetVal.c         void_function.c \
ContDoWhile.c      FnPrtyp.c         IfFloatArith.c        MultRel.c             PassParamDiff.c     t2.c             VoidRet.c \
"

for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done

FOLDER=./test_files

for f in $FILES
do
	echo $f"s306.cfg"
	echo $f"s306.cfg" >> out2
	./cfglp -eval -ast -tokens $FOLDER/$f"s306.cfg"
	# ./cfglp64 -tokens -eval -ast  $FOLDER/$f"s306.cfg"
done

# meld out1 out2

#FloatVal.ecfg