@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i16 0
@b = global i8 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__const1 = alloca i16
	store i16 1, i16* %__const1
	%__tmp1 = load i16, i16* %__const1

	store i16 %__tmp1, i16* @a
	%__const3 = alloca i8
	store i8 4, i8* %__const3
	%__tmp3 = load i8, i8* %__const3

	store i8 %__tmp3, i8* @b
br label %while1.cond

while1.cond:

	%__tmp5 = load i16, i16* @a
	%__tmp6 = load i16, i16* @b
	%cmp1 = icmp ne i16 %__tmp5, %__tmp6
	br i1 %cmp1, label %while1.body, label %while1.end
while1.body:
	%__tmpGlobal_1a = load i16, i16* @a
	%__castGlobal_1a = sext i16 %__tmpGlobal_1a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1a);


	%__tmp8 = load i16, i16* @a
	%__tmp9 = add i16 %__tmp8, 1

	store i16 %__tmp9, i16* @a
	br label %while1.cond
while1.end:

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

