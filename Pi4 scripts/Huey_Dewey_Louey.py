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

users = ["Huey", "Dewey", "Louey"]
addrs = ["10:52:1C:5D:68:CA", "F0:08:D1:D7:AE:16", "F0:08:D1:D3:25:0A"]

    
def getValues(user, addr, attempt = 0):
    print(f"Connecting to {user}...")
    try:
        device   = btle.Peripheral(addr)
        characts = device.getCharacteristics()
        varNames = [['f19','Temperature'],['f20','Pressure'],['f21','Air Humidity'], ['f22','Light'],['f23', 'CO2'],['f24', 'VOC']]
        measList = []
        for char in characts:
            hand = char.getHandle()
            UUID = str(char.uuid)
            for var in varNames:
                if var[0] in UUID:
                    print(var[1], end = " ")
                    val = device.readCharacteristic(hand).decode('utf-8')
                    print(val, end = "\t")
                    measList.append(float(val))
        device.disconnect()
        time.sleep(1)
        print("")
        return measList

        
    except btle.BTLEDisconnectError:
        print(f'Connection error {user}.')
        if attempt < 3:
            print(f'Trying again...')
            measList = getValues(user,addr, attempt + 1)
            return measList

#     except btle.BTLEInternalError:
#             print('other Connection Error')
#             device.disconnect()
#             measList = getValues(user,addr, attemt + 1)
# 
#     except:
#         print('Other error')
        
#     try:
#         characts = device.getCharacteristics()
#         varNames = [['f19','Temperature'],['f20','Pressure'],['f21','Air Humidity'], ['f22','Light'],['f23', 'CO2'],['f24', 'VOC']]
#         measList = []
#         for char in characts:
#             hand = char.getHandle()
#             UUID = str(char.uuid)
#             for var in varNames:
#                 if var[0] in UUID:
#                     print(var[1], end = " ")
#                     val = device.readCharacteristic(hand).decode('utf-8')
#                     print(val, end = "\t")
#                     measList.append(float(val))
#         device.disconnect()
#         time.sleep(1)
#         print("")
        


def writeDB(user, addr, measList):
    if measList != None:
        json_body = [
        {
            "measurement": "Climate",
             "tags":{
                 "location": user,
                 "id"      : addr
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
#     print(json_body)
        client = influxdb.InfluxDBClient(host='localhost', port = 8086)
#     print(client.get_list_database())
        client.switch_database('tomatoTest')
        client.write_points(json_body)

def streamValues(user,addr, timeLen):
    startTime = time.time()
    varNames = [['f19','Temperature'],['f20','Pressure'],['f21','Air Humidity'], ['f22','Light'],['f23', 'CO2'],['f24', 'VOC']]
    measList = []
    
    try:
        print(f"Connecting to {user}...")
        device   = btle.Peripheral(addr)
    except btle.BTLEDisconnectError:
        print('Connection Failed')
        return
        
    while time.time()-startTime < timeLen:
        try:
            measList = []
            characts = device.getCharacteristics()
            for char in characts:
                hand = char.getHandle()
                UUID = str(char.uuid)
                for var in varNames:
                    if var[0] in UUID:
                        print(var[1], end = " ")
                        val = device.readCharacteristic(hand).decode('utf-8')
                        print(val, end = "\t")
                        measList.append(float(val))      
            print("")
            writeDB(user,addr,measList)
            time.sleep(0.5)
#         except BTLEDisconnectError:
#             print('Connection Failed')
#             time.sleep(0.5)
        except KeyboardInterrupt:
            print('Stopped Stream')
            device.disconnect()
            return
        
        except btle.BTLEInternalError:
            print('other Connection Error')
            device.disconnect()
            return
        
    device.disconnect()


if __name__ == "__main__":
    for i in [0,1,2]:
#         try:
            measList = [0,0,0,0,0,0]
            measList = getValues(users[i],addrs[i])
            writeDB(users[i], addrs[i], measList)
            print("")
            time.sleep(1)
#         except:
#             time.sleep(1)

       
# print(values)
#f = open('/home/pi/weatherstation/TT.txt', 'a')
# dev.disconnect()
# time.sleep(2)
#f.write(values)
#f.close()

# client = influxdb.InfluxDBClient(host='localhost', port = 8086)
# print(client.get_list_database())
# client.switch_database('tomatoTest')
# 


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
# client.write_points(json_body)

#print(generateJson(measList))
    

# time.time()

