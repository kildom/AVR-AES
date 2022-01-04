
from tinyec import ec, registry

MAX_SIZE = (521 + 7) // 8 + 1
MAX_BUFS = 32


bufs = []
stack = bytearray()

for i in range(0, MAX_BUFS):
    bufs.append(bytearray(b'\x00' * MAX_SIZE))


def bytes2int(size, b):
    return int.from_bytes(b[0:size], 'little')

def int2bytes(size, b):
    try:
        return bytearray(b.to_bytes(size, 'little'))
    except:
        max = int.from_bytes(b'\x01' + b'\x00' * size, 'big')
        return bytearray((b % max).to_bytes(size, 'little'))

def int2buf(size, index, x):
    b = int2bytes(size, x)
    bufs[index] = bytearray(b + b'\x00' * (MAX_SIZE - size))

def buf2int(size, index):
    return bytes2int(size, bufs[index])

# ===============================

flags = 0
byte = 0

def get_ptr(stack_ref):
    index = stack[-(stack_ref + 1)]
    return bufs[index]

def add(size, r, a, b):
    global flags
    r = get_ptr(r)
    a = get_ptr(a)
    b = get_ptr(b)
    carry = 0
    for i in range(0, size):
        s = a[i] + b[i] + carry
        r[i] = s % 256
        carry = s // 256
    flags <<=  1
    flags |= carry

def sub(size, r, a, b):
    global flags
    r = get_ptr(r)
    a = get_ptr(a)
    b = get_ptr(b)
    carry = 0
    zero = 0
    for i in range(0, size):
        s = a[i] - b[i] - carry
        r[i] = s % 256
        zero |= r[i]
        carry = 1 if s < 1 else 0
    flags <<=  1
    flags |= carry
    flags = (flags & 0xF7)
    if zero:
        flags |= 0x08

def mulbyte(size, r, a, mul256):
    global byte
    r = get_ptr(r)
    a = get_ptr(a)
    carry = 0
    for i in range(0, size):
        if mul256:
            s = a[i] * 256 + carry
        else:
            s = a[i] * byte + carry
        r[i] = s % 256
        carry = s // 256

def divmod(size, v, r, a, b, tmp):
    v = get_ptr(v)
    r = get_ptr(r)
    a = get_ptr(a)
    b = get_ptr(b)
    r[:] = a # TODO: separate instruction if needed elsewhere
    v[:] = b'\x00' * MAX_SIZE # TODO: this is the same as "set 0" instruction, so maybe it can be done before or share code with "set 0"
    

# ===============================

int2buf(4, 0, 4039824)
int2buf(4, 1, 3048923)
int2buf(4, 2, 230934)
stack.append(0)
stack.append(1)
stack.append(2)
sub(3, 0, 1, 2)
print(buf2int(8, 0))
print(buf2int(8, 1))
print(buf2int(8, 2))
print((3048923 - 4039824) % 0x1000000)

