define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%a = sub i32 mul i32 8, 4, 2
	store i32 %a, i32* @a
	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

