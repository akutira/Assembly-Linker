		lw	0	1	five
tist	lw	0	4	Mulab
start	jalr	4	7
		lw	0	1	Tidt
		beq	0	0	tist
done	halt
five	.fill	5