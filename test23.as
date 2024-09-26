Diff	lw	0	1	five
		lw	0	4	SubAdr
Trip    add 1   1   1
Start	jalr	4	7
		sw	0	1	Trip
		lw  0	3	Six
		beq	0	0	Start
done	halt
five	.fill	5
Six		.fill	6
Spec    .fill   Diff
Tank    .fill   Trip
