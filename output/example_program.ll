@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i8 0
@b = global i8 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__const1 = alloca i8
	store i8 6, i8* %__const1
	%__tmp1 = load i8, i8* %__const1

	store i8 %__tmp1, i8* @a

	%cmp1 = icmp slt i8 8, 6
	br i1 %cmp1, label %if1.then, label %if1.else

if1.then:

	%cmp2 = icmp slt i8 6, 2
	br i1 %cmp2, label %if2.then, label %if2.else

if2.then:
	%__const3 = alloca i8
	store i8 2, i8* %__const3
	%__tmp3 = load i8, i8* %__const3

	store i8 %__tmp3, i8* @b
	%__tmpGlobal_1b = load i8, i8* @b
	%__castGlobal_1b = zext i8 %__tmpGlobal_1b to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1b);

	br label %if2.end
if2.else:
	%__const6 = alloca i8
	store i8 1, i8* %__const6
	%__tmp6 = load i8, i8* %__const6

	store i8 %__tmp6, i8* @b
	%__tmpGlobal_2b = load i8, i8* @b
	%__castGlobal_2b = zext i8 %__tmpGlobal_2b to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_2b);

	br label %if2.end
if2.end:

	br label %if1.end
if1.else:
	%__tmpGlobal_3a = load i8, i8* @a
	%__castGlobal_3a = zext i8 %__tmpGlobal_3a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_3a);

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

