# -*- coding: utf-8 -*-
"""
Created on Wed Dec 16 16:27:18 2020

@author: jvissche
"""


from bluepy import btle

print("Connecting to Tomato 1...")

dev = btle.Peripheral("10:52:1C:5D:68:CA")
chars = dev.getCharacteristics()

for char in chars:
    UUID = str(char.uuid)
    
    