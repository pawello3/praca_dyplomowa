#!/usr/bin/python3.5
# -*- coding: utf-8 -*-

from random import randrange, uniform

for i in range(0, 1050000): # > 2^16 * 16
	key = randrange(-2147483647, 2147483647)
	value = uniform(0, 20000000)
	print(key, " ", value)
