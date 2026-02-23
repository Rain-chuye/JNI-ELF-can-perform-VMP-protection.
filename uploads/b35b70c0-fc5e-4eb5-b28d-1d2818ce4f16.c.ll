; ModuleID = '/app/uploads/b35b70c0-fc5e-4eb5-b28d-1d2818ce4f16.c'
source_filename = "/app/uploads/b35b70c0-fc5e-4eb5-b28d-1d2818ce4f16.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.secret_logic.secret = private unnamed_addr constant [31 x i8] c"This is a very secret message!\00", align 16
@.str = private unnamed_addr constant [14 x i8] c"Branch A: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [14 x i8] c"Branch B: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [18 x i8] c"Final result: %d\0A\00", align 1
@.str.3 = private unnamed_addr constant [12 x i8] c"Secret: %s\0A\00", align 1
@.str.4 = private unnamed_addr constant [26 x i8] c"Starting complex test...\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca [31 x i8], align 16
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 16 %5, ptr align 16 @__const.secret_logic.secret, i64 31, i1 false)
  store i32 0, ptr %6, align 4
  %8 = load i32, ptr %3, align 4
  %9 = load i32, ptr %4, align 4
  %10 = icmp sgt i32 %8, %9
  br i1 %10, label %11, label %18

11:                                               ; preds = %2
  %12 = load i32, ptr %3, align 4
  %13 = mul nsw i32 %12, 2
  %14 = load i32, ptr %4, align 4
  %15 = add nsw i32 %13, %14
  store i32 %15, ptr %6, align 4
  %16 = load i32, ptr %6, align 4
  %17 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %16)
  br label %25

18:                                               ; preds = %2
  %19 = load i32, ptr %4, align 4
  %20 = mul nsw i32 %19, 3
  %21 = load i32, ptr %3, align 4
  %22 = sub nsw i32 %20, %21
  store i32 %22, ptr %6, align 4
  %23 = load i32, ptr %6, align 4
  %24 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %23)
  br label %25

25:                                               ; preds = %18, %11
  store i32 0, ptr %7, align 4
  br label %26

26:                                               ; preds = %33, %25
  %27 = load i32, ptr %7, align 4
  %28 = icmp slt i32 %27, 5
  br i1 %28, label %29, label %36

29:                                               ; preds = %26
  %30 = load i32, ptr %7, align 4
  %31 = load i32, ptr %6, align 4
  %32 = add nsw i32 %31, %30
  store i32 %32, ptr %6, align 4
  br label %33

33:                                               ; preds = %29
  %34 = load i32, ptr %7, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, ptr %7, align 4
  br label %26, !llvm.loop !6

36:                                               ; preds = %26
  %37 = load i32, ptr %6, align 4
  %38 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %37)
  %39 = getelementptr inbounds [31 x i8], ptr %5, i64 0, i64 0
  %40 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, ptr noundef %39)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

declare i32 @printf(ptr noundef, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str.4)
  call void @secret_logic(i32 noundef 10, i32 noundef 5)
  call void @secret_logic(i32 noundef 3, i32 noundef 7)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
