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

	%__tmp3 = load i8* @a
	%cmp1 = icmp slt i8 %__tmp3, 12
	br i1 %cmp1, label %if1.then, label %if1.end

if1.then:
br label %while1.cond

while1.cond:

	%__tmp4 = load i8* @a
	%cmp2 = icmp sle i8 %__tmp4, 12
	br i1 %cmp2, label %while1.body, label %while1.end
while1.body:

	%__tmp5 = load i8* @a
	%cmp2 = icmp slt i8 %__tmp5, 12
	br i1 %cmp2, label %if2.then, label %if2.end

if2.then:

	%__tmp6 = load i8* @a
	%__tmp7 = add i8 %__tmp6, 2

	store i8 %__tmp7, i8* @a
	br label %if2.end
if2.end:

	br label %while1.cond
while1.end:

	br label %if1.end
if1.end:

	ret void
}


define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

