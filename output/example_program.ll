@pfmt = constant [4 x i8] c"%d\0A\00"
declare i32 @printf(i8*,...)

@a = global i16 0

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__tmp1 = add i32 2, 2

	store i16 %__tmp1, i16* @a
	%__tmpGlobal_a = load i16, i16* @a
	%__castGlobal_a = sext i16 %__tmpGlobal_a to i32
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_a);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

