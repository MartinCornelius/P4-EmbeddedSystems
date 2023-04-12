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
	store i8 2, i8* %__const1
	%__tmp1 = load i8* %__const1

	store i8 %__tmp1, i8* @a
br label %while1.cond

while1.cond:

	%__tmp3 = load i8* @a
	%__tmp4 = load i8* @a
	%cmp1 = icmp eq i8 %__tmp3, %__tmp4
	br i1 %cmp1, label %while1.body, label %while1.end
while1.body:
br label %while2.cond

while2.cond:

	%__tmp5 = load i8* @a
	%__tmp6 = load i8* @a
	%cmp2 = icmp eq i8 %__tmp5, %__tmp6
	br i1 %cmp2, label %while2.body, label %while2.end
while2.body:
	br label %while2.cond
while2.end:

	br label %while1.cond
while1.end:

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

