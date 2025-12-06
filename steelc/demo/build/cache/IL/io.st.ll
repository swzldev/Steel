; ModuleID = 'io.st'
source_filename = "io.st"

define void @print(ptr %message) {
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
