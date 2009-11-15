#!/usr/bin/python

bytes = raw_input().split(' ')
if len(bytes) != 6:
        raise ValueError
num = 0
for p in range(6):
        num |= int(bytes[p]) << (5 - p)
print 'Table index : %i -> Hex %#x' % (num, num)
