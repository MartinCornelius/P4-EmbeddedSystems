define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%a = 
	store i32 %a, i32* @a
	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

