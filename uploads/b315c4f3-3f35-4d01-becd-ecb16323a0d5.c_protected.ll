; ModuleID = '/app/uploads/b315c4f3-3f35-4d01-becd-ecb16323a0d5.c'
source_filename = "/app/uploads/b315c4f3-3f35-4d01-becd-ecb16323a0d5.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.secret_logic.secret = private unnamed_addr global [31 x i8] c"\21\1d\1c\06\55\1c\06\55\14\55\03\10\07\0c\55\06\10\16\07\10\01\55\18\10\06\06\14\12\10\54\75", align 1
@.str = private unnamed_addr global [14 x i8] c"\d4\e4\f7\f8\f5\fe\b6\d7\ac\b6\b3\f2\9c\96", align 1
@.str.1 = private unnamed_addr global [14 x i8] c"\cb\fb\e8\e7\ea\e1\a9\cb\b3\a9\ac\ed\83\89", align 1
@.str.2 = private unnamed_addr global [18 x i8] c"\13\3c\3b\34\39\75\27\30\26\20\39\21\6f\75\70\31\5f\55", align 1
@.str.3 = private unnamed_addr global [12 x i8] c"\df\e9\ef\fe\e9\f8\b6\ac\a9\ff\86\8c", align 1
@.str.4 = private unnamed_addr global [26 x i8] c"\e4\c3\d6\c5\c3\de\d9\d0\97\d4\d8\da\c7\db\d2\cf\97\c3\d2\c4\c3\99\99\99\bd\b7", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @secret_logic(i32 noundef %r0, i32 noundef %r1) #0 {
  %r3 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r5 = alloca [31 x i8], align 16
  %r6 = alloca i32, align 4
  %r7 = alloca i32, align 4
  %v_state = alloca i32, align 4
  store i32 948627, ptr %v_state, align 4
  br label %v_dispatcher

v_dispatcher:
  %curr_v_state = load i32, ptr %v_state, align 4
  switch i32 %curr_v_state, label %v_default_dest [
    i32 948627, label %v_block_entry
    i32 278308, label %v_block_r11
    i32 609664, label %v_block_r18
    i32 594566, label %v_block_r25
    i32 510316, label %v_block_r26
    i32 233905, label %v_block_r29
    i32 362583, label %v_block_r33
    i32 321106, label %v_block_r36
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
  %next_v_state_entry = select i1 %r10, i32 278308, i32 609664
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
  store i32 594566, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r18:
  %r19 = load i32, ptr %r4, align 4
  %r20 = mul nsw i32 %r19, 3
  %r21 = load i32, ptr %r3, align 4
  %r22 = sub nsw i32 %r20, %r21
  store i32 %r22, ptr %r6, align 4
  %r23 = load i32, ptr %r6, align 4
  %r24 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %r23)
  store i32 594566, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r25:
  store i32 0, ptr %r7, align 4
  store i32 510316, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r26:
  %r27 = load i32, ptr %r7, align 4
  %r28 = icmp slt i32 %r27, 5
  %next_v_state_r26 = select i1 %r28, i32 233905, i32 321106
  store i32 %next_v_state_r26, ptr %v_state, align 4
  br label %v_dispatcher

v_block_r29:
  %r30 = load i32, ptr %r7, align 4
  %r31 = load i32, ptr %r6, align 4
  %r32 = add nsw i32 %r31, %r30
  store i32 %r32, ptr %r6, align 4
  store i32 362583, ptr %v_state, align 4
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
  call void @decrypt_data(ptr @__const.secret_logic.secret, i64 31, i8 117)
  call void @decrypt_data(ptr @.str, i64 14, i8 150)
  call void @decrypt_data(ptr @.str.1, i64 14, i8 137)
  call void @decrypt_data(ptr @.str.2, i64 18, i8 85)
  call void @decrypt_data(ptr @.str.3, i64 12, i8 140)
  call void @decrypt_data(ptr @.str.4, i64 26, i8 183)
  ret void
}
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]
