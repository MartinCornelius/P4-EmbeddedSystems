define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%a = add i32 2, 2
	store i32 %a, i32* @a
	%b = sub i32 6, 1
	store i32 %b, i32* @b
	ret void
}

@a = global i32 0
@b = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

