; ModuleID = 'tests/complex_test.c'
source_filename = "tests/complex_test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.secret_logic.secret = private unnamed_addr global [31 x i8] c"\8b\b7\b6\ac\ff\b6\ac\ff\be\ff\a9\ba\ad\a6\ff\ac\ba\bc\ad\ba\ab\ff\b2\ba\ac\ac\be\b8\ba\fe\df", align 1
@.str = private unnamed_addr global [14 x i8] c"\a1\91\82\8d\80\8b\c3\a2\d9\c3\c6\87\e9\e3", align 1
@.str.1 = private unnamed_addr global [14 x i8] c"\d7\e7\f4\fb\f6\fd\b5\d7\af\b5\b0\f1\9f\95", align 1
@.str.2 = private unnamed_addr global [18 x i8] c"\d6\f9\fe\f1\fc\b0\e2\f5\e3\e5\fc\e4\aa\b0\b5\f4\9a\90", align 1
@.str.3 = private unnamed_addr global [12 x i8] c"\5e\68\6e\7f\68\79\37\2d\28\7e\07\0d", align 1
@.str.4 = private unnamed_addr global [26 x i8] c"\e8\cf\da\c9\cf\d2\d5\dc\9b\d8\d4\d6\cb\d7\de\c3\9b\cf\de\c8\cf\95\95\95\b1\bb", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic(i32 noundef %r0, i32 noundef %r1) #0 {
  %r3 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r5 = alloca [31 x i8], align 16
  %r6 = alloca i32, align 4
  %r7 = alloca i32, align 4
  %v_state = alloca i32, align 4
  store i32 824053, ptr %v_state, align 4
  br label %v_dispatcher

v_dispatcher:
  %curr_v_state = load i32, ptr %v_state, align 4
  switch i32 %curr_v_state, label %v_default_dest [
    i32 824053, label %v_block_entry
    i32 943119, label %v_block_r11
    i32 796295, label %v_block_r18
    i32 694461, label %v_block_r25
    i32 76172, label %v_block_r26
    i32 740532, label %v_block_r29
    i32 502492, label %v_block_r33
    i32 834482, label %v_block_r36
  ]

v_default_dest:
  ret void

v_block_entry:
  store i32 %r0, ptr %r3, align 4
  store i32 %r1, ptr %r4, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 16 %r5, ptr align 16 @__const.secret_logic.secret, i64 31, i1 false)
  store i32 0, ptr %r6, align 4
  %r8 = load i32, ptr %r3, align 4
  %r9 = load i32, ptr %r4, align 4
  %r10 = icmp sgt i32 %r8, %r9
  %next_v_state_entry = select i1 %r10, i32 943119, i32 796295
  store i32 %next_v_state_entry, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r11:
  %r12 = load i32, ptr %r3, align 4
  %r13 = mul nsw i32 %r12, 2
  %r14 = load i32, ptr %r4, align 4
  %r15 = add nsw i32 %r13, %r14
  store i32 %r15, ptr %r6, align 4
  %r16 = load i32, ptr %r6, align 4
  %r17 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %r16)
  store i32 694461, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r18:
  %r19 = load i32, ptr %r4, align 4
  %r20 = mul nsw i32 %r19, 3
  %r21 = load i32, ptr %r3, align 4
  %r22 = sub nsw i32 %r20, %r21
  store i32 %r22, ptr %r6, align 4
  %r23 = load i32, ptr %r6, align 4
  %r24 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %r23)
  store i32 694461, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r25:
  store i32 0, ptr %r7, align 4
  store i32 76172, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r26:
  %r27 = load i32, ptr %r7, align 4
  %r28 = icmp slt i32 %r27, 5
  %next_v_state_r26 = select i1 %r28, i32 740532, i32 834482
  store i32 %next_v_state_r26, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r29:
  %r30 = load i32, ptr %r7, align 4
  %r31 = load i32, ptr %r6, align 4
  %r32 = add nsw i32 %r31, %r30
  store i32 %r32, ptr %r6, align 4
  store i32 502492, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r33:
  %r34 = load i32, ptr %r7, align 4
  %r35 = add nsw i32 %r34, 1
  store i32 %r35, ptr %r7, align 4
  store i32 0, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r36:
  %r37 = load i32, ptr %r6, align 4
  %r38 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %r37)
  %r39 = getelementptr inbounds [31 x i8], ptr %r5, i64 0, i64 0
  %r40 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, ptr noundef %r39)
ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

declare i32 @printf(ptr noundef, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {

  %r1 = alloca i32, align 4
  store i32 0, ptr %r1, align 4
  %r2 = call i32 (ptr, ...) @printf(ptr noundef @.str.4)
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

declare void @decrypt_data(ptr, i64, i8)
define void @__vmp_init_strings() {
entry:
  call void @decrypt_data(ptr @__const.secret_logic.secret, i64 31, i8 223)
  call void @decrypt_data(ptr @.str, i64 14, i8 227)
  call void @decrypt_data(ptr @.str.1, i64 14, i8 149)
  call void @decrypt_data(ptr @.str.2, i64 18, i8 144)
  call void @decrypt_data(ptr @.str.3, i64 12, i8 13)
  call void @decrypt_data(ptr @.str.4, i64 26, i8 187)
  ret void
}
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]
