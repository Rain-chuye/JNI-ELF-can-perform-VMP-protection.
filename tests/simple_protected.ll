; ModuleID = 'tests/simple.c'
source_filename = "tests/simple.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr global [12 x i8] c"\35\03\05\14\03\12\5c\46\43\15\6c\66", align 1
@.str.1 = private unnamed_addr constant [20 x i8] c"ThisIsASecretString\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_func() #0 {
  %args = alloca [4 x ptr]
  %bytecode_ptr = getelementptr inbounds [11 x i8], ptr @bc_secret_func, i64 0, i64 0
  %args_ptr = getelementptr inbounds [4 x ptr], ptr %args, i64 0, i64 0
  call void @vmp_interpreter(ptr %bytecode_ptr, ptr %args_ptr)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %args = alloca [4 x ptr]
  %bytecode_ptr = getelementptr inbounds [11 x i8], ptr @bc_main, i64 0, i64 0
  %args_ptr = getelementptr inbounds [4 x ptr], ptr %args, i64 0, i64 0
  call void @vmp_interpreter(ptr %bytecode_ptr, ptr %args_ptr)
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

@bc_secret_func = private constant [11 x i8] c"\76\5d\77\77\77\77\77\77\77\64\67", align 1
@bc_main = private constant [11 x i8] c"\76\5d\77\77\77\77\77\77\77\64\67", align 1

declare void @vmp_decrypt(ptr, i64, i8)
declare void @vmp_interpreter(ptr, ptr)

define void @__vmp_init() {
entry:
  call void @vmp_decrypt(ptr @.str, i64 12, i8 102)
  ret void
}

@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init, ptr null }]
