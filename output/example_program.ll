@pfmt = constant [5 x i8] c"%ld\0A\00"
declare i32 @printf(i8*,...)

define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__tmp1 = mul i32 4, 3
	%__tmp2 = mul i32 1, 2
	%__tmp3 = sub i32 %__tmp2, %__tmp1

	store i32 %__tmp3, i32* @a
	%__tmpGlobal_a = load i32, i32* @a
	call i32(i8*,...) @printf(i8* getelementptr([5 x i8], [5 x i8]* @pfmt, i32 0, i32 0), i32 %__tmpGlobal_a);

	ret void
}

@a = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

