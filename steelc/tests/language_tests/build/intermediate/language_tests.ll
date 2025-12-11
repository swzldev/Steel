; ModuleID = 'C:\Users\maxmt\source\repos\Steel\steelc\tests\language_tests\build\intermediate\IR\src\main.st.ll'
source_filename = "main.st"

@0 = private unnamed_addr constant [15 x i8] c"Hello, Steel!\0A\00", align 1

define i32 @main() {
entry:
  %ret.slot = alloca i32, align 4
  call void @_ZN5tests2io5printEPc(ptr @0)
  store i32 0, ptr %ret.slot, align 4
  br label %scope.clean

return:                                           ; preds = %scope.clean
  %0 = load i32, ptr %ret.slot, align 4
  ret i32 %0

scope.clean:                                      ; preds = %entry
  br label %return
}

define void @_ZN5tests2io5printEPc(ptr %message) {
entry:
  %message1 = alloca ptr, align 8
  store ptr %message, ptr %message1, align 8
  %0 = load ptr, ptr %message1, align 8
  %1 = call i32 (ptr, ...) @printf(ptr %0)
  br label %scope.clean

return:                                           ; preds = %scope.clean
  ret void

scope.clean:                                      ; preds = %entry
  br label %return
}

declare i32 @printf(ptr, ...)
