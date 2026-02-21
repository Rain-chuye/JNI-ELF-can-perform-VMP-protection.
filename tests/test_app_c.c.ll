; ModuleID = 'tests/test_app_c.c'
source_filename = "tests/test_app_c.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [31 x i8] c"This is a super secret string!\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"Running secret logic...\0A\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"Secret: %s\0A\00", align 1
@.str.3 = private unnamed_addr constant [30 x i8] c"Hello from unprotected part!\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic() #0 {
  %1 = alloca ptr, align 8
  store ptr @.str, ptr %1, align 8
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str.1)
  %3 = load ptr, ptr %1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, ptr noundef %3)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str.3)
  call void @secret_logic()
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
