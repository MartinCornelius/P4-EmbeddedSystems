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
	store i8 1, i8* %__const1
	%__tmp1 = load i8* %__const1

	store i8 %__tmp1, i8* @a
	%__tmpGlobal_1a = load  i8* @a
	%__castGlobal_1a = sext i8 %__tmpGlobal_1a to i32
	call i32(i8*,...)* @printf(i8* getelementptr([4 x i8]* @pfmt, i32 0, i32 0), i32 %__castGlobal_1a);

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

