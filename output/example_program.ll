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
	store i8 6, i8* %__const1
	%__tmp1 = load i8, i8* %__const1

	store i8 %__tmp1, i8* @a

	%cmp1 = icmp slt i8 3, 6
	br i1 %cmp1, label %if1.then, label %if1.cond

if1.then:
	%__tmpGlobal_1a = load i8, i8* @a
	%__castGlobal_1a = zext i8 %__tmpGlobal_1a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1a);

	br label %if1.end
if1.cond:

	%cmp2 = icmp eq i8 5, 8
	br i1 %cmp2, label %if2.then, label %if2.cond

if2.then:
	%__tmpGlobal_2a = load i8, i8* @a
	%__castGlobal_2a = zext i8 %__tmpGlobal_2a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_2a);

	br label %if1.end
if2.cond:

	%cmp3 = icmp eq i8 34, 2
	br i1 %cmp3, label %if3.then, label %if3.cond

if3.then:
	%__tmp5 = add i8 2, 2

	store i8 %__tmp5, i8* @a
	br label %if1.end
if3.cond:

	%cmp4 = icmp eq i8 34, 2
	br i1 %cmp4, label %if4.then, label %if4.else

if4.then:
	%__tmp7 = add i8 2, 2

	store i8 %__tmp7, i8* @a
	br label %if1.end
if4.else:
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

