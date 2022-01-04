```
X = result
Y = a
Z = b
P = c


CLR carry
MOV index, size
MOVW tmp, X
begin_of_loop:
    LD Y+, a
    LD Z+, b
    ADD a, b
    CLR b
    ROL b
    ADD a, carry
    ADDC b, zero
    ST X+, a
    MOV carry, b
    SUBI index, 1
BRCC begin_of_loop
MOVW X, tmp
MOVW Y, mod

CLR carry
MOV index, size
MOVW tmp, X
begin_of_loop:
    LD Y+, a
    LD Z+, b
    ADD a, b
    CLR b
    ROL b
    ADD a, carry
    ADDC b, zero
    ST X+, a
    MOV carry, b
    SUBI index, 1
BRCC begin_of_loop
MOVW X, tmp
MOVW Y, mod

mod_sub:
    sub r = a - b
    add tmp = r + mod
    if carry: swap tmp, r
    ret

mod_add:
    add r = a + b
    sub tmp = r - mod
    if old_carry || !carry: swap tmp, r
    ret

mod_mul:
    zero res
    forbits b:
        mod_add tmp2 = res + a
        if old_old_carry: swap tmp2, res
        mod_add a = a + a

mod_mul:
    zero res
    forbytes_begin  // push two bytes: 0 index and undefined byte to stack
    forbytes_update b, endloop // increment index on stack, update byte on stack with new one, if no more, jump to label
    // OR maybe set flag and do jump with "if .. goto"
        mulbyte+1 tmp = a * byte
        add+1 res = res + tmp
        divmod+1 tmp = res / mod, res = res % mod // mod value has many `1` on the beginning, so mod will be subtracted only once here during modulo
                                                  // (this should be confirmed)
                                                  // with this knowledge, we first calculated maximum `shift left` of mod and stop when subtraction was successful
        mulbyte+1 a = a * 256
        divmod+1 tmp = a / mod, a = a % mod
    endloop

mul+1:
    zero+1 res
    forbytes b:
        mulbyte+1 tmp = a * byte
        add+1 res = res + tmp
        mulbyte+1 a = a * 256

mod_remove_sign:
    add+1 tmp = res + mod
    if carry: swap tmp, res


extended_gcd: old_r, r => old_s
    set s = 0
    set old_s = 1
    loop_begin:
        divmod tmp2 = old_r / r, tmp3 = old_r % r
        mul+1 tmp2 = tmp2 * s
        sub+1 tmp2 = old_s - tmp2
        swap old_s, s
        swap tmp2, s
        swap old_r, r
        swap tmp3, r
        if old_old_carry: goto loop_begin
    mod_remove_sign old_s = old_s % mod

point_m: px py qx qy => res
    sub tmp = px - qx
    if zero: goto equal
        mod_sub tmp = (px - qx) % p
        extended_gcd res = (1 / tmp) % p
        mod_sub tmp = (py - px) % p
        mod_mul res = tmp * res
        ret
    equal:
        mod_add tmp = py * py
        extended_gcd res = (1 / tmp) % p
        mod_mul tmp = px * px
        mod_add tmp = tmp + tmp
        mod_add tmp = tmp + tmp
        mod_add tmp = tmp + curve_a
        mod_mul res = tmp * res

point_add: px py qx qy => rx ry
    TODO: px==qx && py!=py => return Inf
    point_m m = point_m(px py qx qy)
    mod_mul rx = m * m
    mod_sub rx = rx - px
    mod_sub rx = rx - qx
    mod_sub ry = px - rx
    mod_mul ry = m * ry
    mod_sub ry = ry - py

divmod: v = a / b, r = a % b, tmp
    set v = 0
    set r = a
    div_shift_init b = b << N
    loop_begin:
        sub+1 tmp = r - b
        if not carry:
            swap tmp, r
        div_set_res_bit v[...] = !carry, carry = last bit
        if carry:
            ret
        div_shift_bit b = b >> 1
        goto loop_begin
    


sub       r = a - b
add       r = a + b
divmod    v = a / b, r = a % b, tmp
mulbyte   r = a * ?
gotoif    bit, not, location
ret       remove_stack_elements
call      location, copy_stack_1, copy_stack_2, ...
swap      a, b
set       r = value
forbytes_begin
forbytes_update  a, location



r7 - flags
S
T
U - stack
V - unchange by vm
W - unchange by vm
X - unchange by vm
Y - unchange by vm
Z - location

next_instr:
# TODO: push and pop needed registers before going into instruction
LPM r2, Z+
# TODO: SKIP IF not needed
LPM r3, Z+
# TODO: do not restore Z for jump instructions

:bit tree 0000 xxxx    params (2)
MOVW V, X
MOVW W, Y
CALL load_ptr
MOVW Y, Z
SWAP r3
CALL load_ptr
MOVW X, Z
BR next_instr

:bit tree 0001 xxxx     sub (1)
CALL load_ptr
... SUB
BR next_instr

:bit tree 0010 xxxx     add (1)
CALL load_ptr
... ADD
BR next_instr

:bit tree 0011 xxxx     divmod (1)
CALL load_ptr
... DIVMOD
BR next_instr

:bit tree 0011 xxxx     mulbyte (1)
CALL load_ptr
... MULBYTE
BR next_instr

:bit tree 010x xxxx     set0/1 (1)
CALL load_ptr
... SET 0/1
BR next_instr

:bit tree 0110 xxxx     forbytes_update (2)
CALL load_ptr
... forbytes_update
BR next_instr

:bit tree 011x

:bit tree 1111 xxxx     ret (1)
AND r2, 0x0F
ADD U.lo, r2
ADDC U.hi, __zero__
MOVW X, U
LD Z.lo, X+
LD Z.hi, X+
BR next_instr

:bit tree 0110 xxxx     forbytes_begin (1)
MOVW X, U
ST -X, __zero__
ST -X, __zero__
MOVW U, X
BR next_instr

:bit tree 0110 xxxx     swap (2)
CALL load_ptrptr
MOV X, Z
SWAP r3
CALL load_ptrptr
LD r2, Z
LD r3, Z+1
LD r4, X
LD r5, X+1
ST Z+, r4
ST Z+, r5
ST X+, r2
ST X+, r3
BR next_instr


:bit tree 011x xxxx     gotoif (2)
MOV r4, r7
SKIP r2, bit 4 clr
NEG r4
AND r4, r2
AND r4, 0x0F
CMP r4, __zero__
BREQ next_instr
MOV r4, __zero__
SKIP r3, bit 7 clr
NEG r4
ADD Z.lo, r3
ADDC Z.hi, r4
BR next_instr

:bit tree 1111 xxxx     call (2+)
AND r2, 0x0F   # TODO: if opcode will be 0000 xxxx this line may be removed
loop_begin:
LD r4, Z+
CALL push_param
SWAP r4
CALL push_param
SUBI r2, 1
BR_IF_NOT_OVERFLOW loop_begin
MOV r4, __zero__
SKIP r3, bit 7 clr
NEG r4
ADD Z.lo, r3
ADDC Z.hi, r4
BR next_instr

-params
-sub       r = a - b
-add       r = a + b
-divmod    v = a / b, r = a % b, tmp
-mulbyte   r = a * ?
-gotoif    bit, not, location
-ret       remove_stack_elements
-call      location, copy_stack_1, copy_stack_2, ...
-swap      a, b
-set       r = value
-forbytes_begin
-forbytes_update  a, location


```

Nice example: https://github.com/alexmgr/tinyec/blob/master/tinyec/ec.py
