@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i32 0
@b = global i32 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__const1 = alloca i32
	store i32 5, i32* %__const1
	%__tmp1 = load i32, i32* %__const1

	store i32 %__tmp1, i32* @a
	%__const3 = alloca i32
	store i32 4, i32* %__const3
	%__tmp3 = load i32, i32* %__const3

	store i32 %__tmp3, i32* @b
	%__tmp5 = load i32, i32* @a
	%__tmp6 = load i32, i32* @b
	%__tmp7 = add i32 %__tmp6, %__tmp5

	store i32 %__tmp7, i32* @b
	%__tmpGlobal_1b = load i32, i32* @b
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__tmpGlobal_1b);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

