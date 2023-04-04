define void @setup() {
entry:
	ret void
}

define void @mainloop() {
entry:
	%cmp1 = icmp slt i32 2, 12
	br i1 %cmp1, label %if1.then, label %if2.cond

if1.then:
	%a = add i32 2, 56
	store i32 %a, i32* @a
	br label %if1.end
if2.cond:
	%cmp2 = icmp sge i32 12, 3
	br i1 %cmp2, label %if2.then, label %if3.cond

if2.then:
	%b = sub i32 3, 6
	store i32 %b, i32* @b
	br label %if1.end
if3.cond:
	%cmp3 = icmp sle i32 6, 1
	br i1 %cmp3, label %if3.then, label %if3.else

if3.then:
	%c = mul i32 5, 3
	store i32 %c, i32* @c
	br label %if1.end
if3.else:
	%d = sdiv i32 9, 3
	store i32 %d, i32* @d
	br label %if1.end
if1.end:
	ret void
}

@a = global i32 0
@b = global i32 0
@c = global i32 0
@d = global i32 0

define i32 @main() {
entry:
	call void @setup()
	call void @mainloop()
	ret i32 0
}

