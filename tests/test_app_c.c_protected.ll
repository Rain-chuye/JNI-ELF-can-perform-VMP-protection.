; ModuleID = 'tests/test_app_c.c'
source_filename = "tests/test_app_c.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr global [31 x i8] c"\16\2a\2b\31\62\2b\31\62\23\62\31\37\32\27\30\62\31\27\21\30\27\36\62\31\36\30\2b\2c\25\63\42", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"Running secret logic...\0A\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"Secret: %s\0A\00", align 1
@.str.3 = private unnamed_addr constant [30 x i8] c"Hello from unprotected part!\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic() #0 {
  %bytecode_ptr = getelementptr inbounds [2 x i8], ptr @vm_code_secret_logic, i64 0, i64 0
  call void @vm_interpreter(ptr %bytecode_ptr, ptr null)
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

@vm_code_secret_logic = private constant [2 x i8] c"\76\75", align 1

declare void @decrypt_data(ptr, i64, i8)
declare void @vm_interpreter(ptr, ptr)

define void @__vmp_init_strings() {
entry:
  call void @decrypt_data(ptr @.str, i64 31, i8 66)
  ret void
}

@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]
