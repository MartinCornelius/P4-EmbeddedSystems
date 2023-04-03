define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
br label %while1.cond

while1.cond:
	%cmp1 = icmp slt i32 2, 3
	br i1 %cmp1, label %while1.body, label %while1.end
while1.body:
	%a = add i32 2, 2
	store i32 %a, i32* @a
	br label %while1.cond
while1.end:
	%b = sub i32 3, 21
	store i32 %b, i32* @b
	ret void
}

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

@a = global i32 0
@b = global i32 0
