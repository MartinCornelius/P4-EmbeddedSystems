define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%cmp1 = icmp eq i32 2, 12
	br i1 %cmp1, label %if1.then, label %if2.cond

if1.then:
	%a = add i32 2, 56
	store i32 %a, i32* @a
	br label %if1.end
if2.cond:
	%cmp2 = icmp slt i32 2, 8
	br i1 %cmp2, label %if2.then, label %if3.cond

if2.then:
	%bob = sub i32 4, 2
	store i32 %bob, i32* @bob
	br label %if1.end
if3.cond:
	%cmp3 = icmp sle i32 2, 8
	br i1 %cmp3, label %if3.then, label %if3.else

if3.then:
	%carl = sub i32 4, 2
	store i32 %carl, i32* @carl
	br label %if1.end
if3.else:
	%y = mul i32 2, 5
	store i32 %y, i32* @y
	br label %if1.end
if1.end:
	ret void
}

@a = global i32 0
@bob = global i32 0
@carl = global i32 0
@y = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

