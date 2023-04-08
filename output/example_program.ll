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
	%__tmp1 = add i8 2, 6

	store i8 %__tmp1, i8* @a

	%__tmp3 = load i8* @a
	%__tmp4 = add i8 %__tmp3, 1
	%__tmp5 = sub i8 %__tmp4, 18

	store i8 %__tmp5, i8* @b
	%__tmpGlobal_1b = load  i8* @b
	%__castGlobal_1b = sext i8 %__tmpGlobal_1b to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1b);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

