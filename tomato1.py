# -*- coding: utf-8 -*-
"""
Created on Wed Dec 16 16:27:18 2020

@author: jvissche
"""


from bluepy import btle
import time
print("Connecting to Tomato 1...")

dev = btle.Peripheral("10:52:1C:5D:68:CA")
chars = dev.getCharacteristics()

variables = [['f22','Temperature'],['f23','Pressure'],['f24','Air Humidity'], ['f25','Soil Humidity']]
values  = '\n' + str(int(time.time())) + ','
for char in chars:
    hand = char.getHandle()
    UUID = str(char.uuid)
    for var in variables:
        if var[0] in UUID:
            print(var[1])
            val = dev.readCharacteristic(hand).decode('utf-8')
            values += str(val) + ','
            print(val)
print(values)
f = open('/weatherstation/Tomato1.txt', 'a+')
f.write(values)
f.close()