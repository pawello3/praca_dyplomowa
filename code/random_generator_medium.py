#!/usr/bin/python3.5
# -*- coding: utf-8 -*-

from random import randrange, uniform

for i in range(0, 530000): # > 2^16 * 8
	key = randrange(-2147483647, 2147483647)
	value = uniform(0, 20000000)
	print(key, " ", value)
