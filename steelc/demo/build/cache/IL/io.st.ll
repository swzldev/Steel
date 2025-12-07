; ModuleID = 'io.st'
source_filename = "io.st"

@0 = private unnamed_addr constant [20 x i8] c"Hi from io module!\0A\00", align 1

define void @_ZN2io6say_hiE() {
entry:
  call void @_ZN2io5printEPc(ptr @0)
  br label %scope.clean

return:                                           ; preds = %scope.clean
  ret void

scope.clean:                                      ; preds = %entry
  br label %return
}

declare void @_ZN2io5printEPc(ptr)
