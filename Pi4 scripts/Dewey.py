# -*- coding: utf-8 -*-
"""
Created on Wed Dec 16 16:27:18 2020

@author: jvissche
"""
import os, sys
from bluepy import btle

os.chdir('/home/pi/.local/lib/python3.7/site-packages/influxdb')
print(os.getcwd())
import influxdb

os.chdir('/home/pi')
import time
import datetime
import json

       
user = "Tomato1"
print(f"Connecting to {user}...")
dev = btle.Peripheral("10:52:1C:5D:68:CA")
chars = dev.getCharacteristics()

variables = [['f19','Temperature'],['f20','Pressure'],['f21','Air Humidity'], ['f22','Light'],['f23', 'CO2'],['f24', 'VOC']]
values  = '\n' + str(int(time.time())) + ','
measList = []
for char in chars:
    hand = char.getHandle()
    UUID = str(char.uuid)
    for var in variables:
        if var[0] in UUID:
            print(var[1])
            val = dev.readCharacteristic(hand).decode('utf-8')
            values += str(val) + ','
            print(val)
            measList.append(float(val))
# print(values)
#f = open('/home/pi/weatherstation/TT.txt', 'a')
dev.disconnect()
time.sleep(2)
#f.write(values)
#f.close()

client = influxdb.InfluxDBClient(host='localhost', port = 8086)
print(client.get_list_database())
client.switch_database('tomatoTest')

json_body = [
    {
        "measurement": "Climate",
         "tags":{
             "location": "Tomato1",
             "id":"10:52:1C:5D:68:CA"
        },
     "time" : (datetime.datetime.now()-datetime.timedelta(hours =2)).strftime("%Y-%m-%dT%H:%M:%S"),
     "fields" : {
         "temperature" : measList[0],
         "pressure"    : measList[1],
         "air Humidity": measList[2],
         "Light"       : measList[3],
         "CO2"         : measList[4],
         "tVOC"        : measList[5]
                 }
     }
    ]

# json_body = [
#     {
#         "measurement": "brushEvents",
#         "tags": {
#             "user": "Carol",
#             "brushId": "6c89f539-71c6-490d-a28d-6c5d84c0ee2f"
#         },
#         "time": "2018-03-28T8:01:00Z",
#         "fields": {
#             "duration": 127
#         }
#         }
#         
# ]
client.write_points(json_body)

#print(generateJson(measList))
    

time.time()

