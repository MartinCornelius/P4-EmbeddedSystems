@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i8 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__const1 = alloca i8
	store i8 2, i8* %__const1
	%__tmp1 = load i8* %__const1

	store i8 %__tmp1, i8* @a

	%__tmp3 = load i8* @a
	%cmp1 = icmp sgt i8 %__tmp3, 12
	br i1 %cmp1, label %if1.then, label %if1.else

if1.then:
	%__tmpGlobal_1a = load  i8* @a
	%__castGlobal_1a = sext i8 %__tmpGlobal_1a to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1a);


	%__tmp5 = load i8* @a
	%cmp2 = icmp sgt i8 %__tmp5, 12
	br i1 %cmp2, label %if2.then, label %if2.else

if2.then:
	%__tmpGlobal_2a = load  i8* @a
	%__castGlobal_2a = sext i8 %__tmpGlobal_2a to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_2a);

	br label %if2.end
if2.else:
	%__tmpGlobal_3a = load  i8* @a
	%__castGlobal_3a = sext i8 %__tmpGlobal_3a to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_3a);

	br label %if2.end
if2.end:

	br label %if2.end
if1.else:
	%__tmpGlobal_4a = load  i8* @a
	%__castGlobal_4a = sext i8 %__tmpGlobal_4a to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_4a);

	br label %if1.end
if1.end:

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

