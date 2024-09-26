Diff	lw	    0	1	five
		lw	    0	4	SubAdr
        add     1   1   1
Start	jalr	4	7
		sw	    0	1	Trip
PASS	lw      0	3	Six
neo		beq	    0	0	Start
Tank    sw      3   5   Undeft
        lw      5   3   Undeft
        beq     2   2   neo
done	halt
five	.fill	5
Six		.fill	6
Spec    .fill   Diff
pac     .fill   Trip
New     .fill   Undefd
Spin    .fill   Undefd
wer     .fill   pac
Shad    .fill   Spec