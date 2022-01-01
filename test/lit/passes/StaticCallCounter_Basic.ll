; RUN:  opt --enable-new-pm=0 -load %passlib --legacy-static-cc -analyze %S/../../inputs/CallCounterInput.ll \
; RUN:   | FileCheck %s
; RUN:  opt -load-pass-plugin %passlib -passes="print<static-cc>" -disable-output \
; RUN:   %S/../../inputs/CallCounterInput.ll 2>&1 | FileCheck %s

; Test StaticCallCounter when run through opt using - basic function calls

; CHECK: foo                  3
; CHECK: bar                  2
; CHECK: fez                  1
