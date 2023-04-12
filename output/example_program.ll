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

	%__tmp7 = load i8* @a
	%__tmp8 = load i8* @a
	%cmp3 = icmp eq i8 %__tmp7, %__tmp8
	br i1 %cmp3, label %if1.then, label %if1.end

if1.then:

	%__tmp9 = load i8* @a
	%__tmp10 = load i8* @a
	%cmp4 = icmp eq i8 %__tmp9, %__tmp10
	br i1 %cmp4, label %if2.then, label %if2.end

if2.then:
	br label %if2.end
if2.end:

	br label %if1.end
if1.end:

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

