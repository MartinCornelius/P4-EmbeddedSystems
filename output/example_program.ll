define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%tmp1 = alloca i32
	store i32 4, i32* %tmp1
	store i32 %tmp1, i32* @a
	%tmp2 = alloca i32
	store i32 5, i32* %tmp2
	store i32 %tmp2, i32* @b
	%tmp3 = alloca i32
	store i32 8, i32* %tmp3
	store i32 %tmp3, i32* @c
	ret void
}

@a = global i32 0
@b = global i32 0
@c = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

