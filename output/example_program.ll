define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%__tmp1 = mul i32 5, 5
	%__tmp2 = sdiv i32 %__tmp1, 5
	%__tmp3 = mul i32 1, 2
	%__tmp4 = add i32 %__tmp3, 2
	%__tmp5 = sub i32 %__tmp4, %__tmp2

	store i32 %__tmp5, i32* @a
	ret void
}

@a = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

