define void @setup() {
entry:
	%b = mul i32 3, 3
	store i32 %b, i32* @b
	ret void
}

define void @mainloop() {
entry:
	%a = add i32 2, 2
	store i32 %a, i32* @a
	ret void
}

@b = global i32 0
@a = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

