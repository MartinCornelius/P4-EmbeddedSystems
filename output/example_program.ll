@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i16 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__const1 = alloca i16
	store i16 100, i16* %__const1
	%__tmp1 = load i16, i16* %__const1

	store i16 %__tmp1, i16* @a
br label %while1.cond

while1.cond:

	%__tmp3 = load i16, i16* @a
	%cmp1 = icmp sgt i16 %__tmp3, 0
	br i1 %cmp1, label %while1.body, label %while1.end
while1.body:
	%__tmpGlobal_1a = load i16, i16* @a
	%__castGlobal_1a = sext i16 %__tmpGlobal_1a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1a);


	%__tmp5 = load i16, i16* @a
	%__tmp6 = sdiv i16 %__tmp5, 2

	store i16 %__tmp6, i16* @a
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

