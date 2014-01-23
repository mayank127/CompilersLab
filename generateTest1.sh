#!/bin/bash
FILES="ContDoWhile.c GlobTernaryInIfLoop.c InfiniteLoop.c MultRelInTernary.c NestedLoop.c RelInCond.c RelInStmt.c test.c Glob.c iftmp.c MultRel.c NestedIf.c NotEqInIf.c RelInIfNotEq.c TempInIfRel.c TwoRelInTernary.c"
for f in $FILES
do
	echo $f
	make -f Makefile.cfg FILE=$f
done