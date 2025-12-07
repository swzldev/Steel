; ModuleID = 'Main.st'
source_filename = "Main.st"

@0 = private unnamed_addr constant [15 x i8] c"Hello, Steel!\0A\00", align 1

define i32 @main() {
entry:
  %ret.slot = alloca i32, align 4
  store i32 0, ptr %ret.slot, align 4
  br label %scope.clean

return:                                           ; preds = %scope.clean
  %0 = load i32, ptr %ret.slot, align 4
  ret i32 %0

scope.clean:                                      ; preds = %entry
  br label %return
}
