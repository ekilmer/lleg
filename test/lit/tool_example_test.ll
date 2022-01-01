; RUN: %tooltestexe %S/../inputs/CallCounterInput.ll 2>&1 | FileCheck %s

; Test StaticCallCounter when run via standalone tool.

; CHECK: foo                  3
; CHECK: bar                  2
; CHECK: fez                  1
