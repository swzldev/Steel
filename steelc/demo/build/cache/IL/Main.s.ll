; ModuleID = 'Main.s'
source_filename = "Main.s"

@0 = private unnamed_addr constant [15 x i8] c"Hello, World!\0A\00", align 1
@1 = private unnamed_addr constant [20 x i8] c"I is 50. Breaking.\0A\00", align 1

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

define i32 @main() {
entry:
  %i = alloca i32, align 4
  %ret.slot = alloca i32, align 4
  store i32 100, ptr %i, align 4
  br label %while.cond

return:                                           ; preds = %scope.clean
  %0 = load i32, ptr %ret.slot, align 4
  ret i32 %0

scope.clean:                                      ; preds = %while.merge
  br label %return

while.cond:                                       ; preds = %if.merge, %entry
  %1 = load i32, ptr %i, align 4
  %2 = icmp sgt i32 %1, 0
  br i1 %2, label %while.body, label %while.merge

while.body:                                       ; preds = %while.cond
  call void @print(ptr @0)
  %3 = load i32, ptr %i, align 4
  %4 = icmp eq i32 %3, 50
  br i1 %4, label %if.then, label %if.merge

while.merge:                                      ; preds = %if.then, %while.cond
  store i32 0, ptr %ret.slot, align 4
  br label %scope.clean

scope.clean1:                                     ; No predecessors!
  unreachable

if.then:                                          ; preds = %while.body
  call void @print(ptr @1)
  br label %while.merge

scope.clean2:                                     ; No predecessors!
  unreachable

if.merge:                                         ; preds = %while.body
  %old = load i32, ptr %i, align 4
  %inc = add i32 %old, -1
  store i32 %inc, ptr %i, align 4
  br label %while.cond
}
