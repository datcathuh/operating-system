[bits 16]
global enable_a20

enable_a20:
	in   al, 0x64         ; read status
.wait1:
	test al, 2            ; input buffer full?
	jnz  .wait1
	mov  al, 0xD1         ; command: write output port
	out  0x64, al

.wait2:
	in   al, 0x64
	test al, 2
	jnz  .wait2
	mov  al, 0xDF         ; set bit 1 (A20 enable)
	out  0x60, al
	ret
