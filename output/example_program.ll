define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

