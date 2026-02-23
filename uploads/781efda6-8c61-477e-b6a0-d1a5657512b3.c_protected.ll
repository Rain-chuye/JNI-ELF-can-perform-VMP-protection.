; ModuleID = '/app/uploads/781efda6-8c61-477e-b6a0-d1a5657512b3.c'
source_filename = "/app/uploads/781efda6-8c61-477e-b6a0-d1a5657512b3.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

@__const.secret_logic.secret = private unnamed_addr global [31 x i8] c"\7b\47\46\5c\0f\46\5c\0f\4e\0f\59\4a\5d\56\0f\5c\4a\4c\5d\4a\5b\0f\42\4a\5c\5c\4e\48\4a\0e\2f", align 1
@.str = private unnamed_addr global [14 x i8] c"\0a\3a\29\26\2b\20\68\09\72\68\6d\2c\42\48", align 1
@.str.1 = private unnamed_addr global [14 x i8] c"\ed\dd\ce\c1\cc\c7\8f\ed\95\8f\8a\cb\a5\af", align 1
@.str.2 = private unnamed_addr global [18 x i8] c"\b5\9a\9d\92\9f\d3\81\96\80\86\9f\87\c9\d3\d6\97\f9\f3", align 1
@.str.3 = private unnamed_addr global [12 x i8] c"\5b\6d\6b\7a\6d\7c\32\28\2d\7b\02\08", align 1
@.str.4 = private unnamed_addr global [26 x i8] c"\d5\f2\e7\f4\f2\ef\e8\e1\a6\e5\e9\eb\f6\ea\e3\fe\a6\f2\e3\f5\f2\a8\a8\a8\8c\86", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic(i32 noundef %r0, i32 noundef %r1) #0 {
  %r3 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r5 = alloca [31 x i8], align 1
  %r6 = alloca i32, align 4
  %r7 = alloca i32, align 4
  %v_state = alloca i32, align 4
  store i32 933177, ptr %v_state, align 4
  br label %v_dispatcher

v_dispatcher:
  %curr_v_state = load i32, ptr %v_state, align 4
  switch i32 %curr_v_state, label %v_default_dest [
    i32 933177, label %v_block_entry
    i32 617280, label %v_block_r11
    i32 155309, label %v_block_r18
    i32 743318, label %v_block_r25
    i32 523387, label %v_block_r26
    i32 744521, label %v_block_r29
    i32 37005, label %v_block_r33
    i32 99150, label %v_block_r36
  ]

v_default_dest:
  ret void

v_block_entry:
  store i32 %r0, ptr %r3, align 4
  store i32 %r1, ptr %r4, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %r5, ptr align 1 @__const.secret_logic.secret, i64 31, i1 false)
  store i32 0, ptr %r6, align 4
  %r8 = load i32, ptr %r3, align 4
  %r9 = load i32, ptr %r4, align 4
  %r10 = icmp sgt i32 %r8, %r9
  %next_v_state_entry = select i1 %r10, i32 617280, i32 155309
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
  store i32 743318, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r18:
  %r19 = load i32, ptr %r4, align 4
  %r20 = mul nsw i32 %r19, 3
  %r21 = load i32, ptr %r3, align 4
  %r22 = sub nsw i32 %r20, %r21
  store i32 %r22, ptr %r6, align 4
  %r23 = load i32, ptr %r6, align 4
  %r24 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %r23)
  store i32 743318, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r25:
  store i32 0, ptr %r7, align 4
  store i32 523387, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r26:
  %r27 = load i32, ptr %r7, align 4
  %r28 = icmp slt i32 %r27, 5
  %next_v_state_r26 = select i1 %r28, i32 744521, i32 99150
  store i32 %next_v_state_r26, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r29:
  %r30 = load i32, ptr %r7, align 4
  %r31 = load i32, ptr %r6, align 4
  %r32 = add nsw i32 %r31, %r30
  store i32 %r32, ptr %r6, align 4
  store i32 37005, ptr %v_state, align 4
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

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}

declare void @decrypt_data(ptr, i64, i8)
define void @__vmp_init_strings() {
entry:
  call void @decrypt_data(ptr @__const.secret_logic.secret, i64 31, i8 47)
  call void @decrypt_data(ptr @.str, i64 14, i8 72)
  call void @decrypt_data(ptr @.str.1, i64 14, i8 175)
  call void @decrypt_data(ptr @.str.2, i64 18, i8 243)
  call void @decrypt_data(ptr @.str.3, i64 12, i8 8)
  call void @decrypt_data(ptr @.str.4, i64 26, i8 134)
  ret void
}
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]
