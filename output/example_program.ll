@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i8 0
@b = global i16 0
@c = global i8 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__tmp1 = sdiv i8 2, 2

	store i8 %__tmp1, i8* @a

	%__tmp3 = mul i16 2, 5
	%__tmp4 = sub i16 %__tmp3, 1

	store i16 %__tmp4, i16* @b
	%__tmpGlobal_b = load i16, i16* @b
	%__castGlobal_b = sext i16 %__tmpGlobal_b to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_b);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

