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
	%__tmp1 = sub i32 2, 3

	store i32 %__tmp1, i32* @a

	%__tmpGlobal_a = load i32, i32* @a
	call i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %__tmpGlobal_a);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

