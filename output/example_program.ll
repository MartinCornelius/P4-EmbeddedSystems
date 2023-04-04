define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
br label %while1.cond

while1.cond:
	%cmp1 = 5	br i1 %cmp1, label %while1.body, label %while1.end
while1.body:
	%first = sub i32 , 
	store i32 %first, i32* @first
	br label %while1.cond
while1.end:



	ret void
}

@first = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

