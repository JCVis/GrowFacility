import os
import time

os.chdir("/home/pi/weatherstation/")
print(os.getcwd())

import Huey_Dewey_Louey as HDL

i = 2
# for j in range(20):
#     measList = [0,0,0,0,0,0]
#     measList = HDL.getValues(HDL.users[i],HDL.addrs[i])
#     HDL.writeDB(HDL.users[i], HDL.addrs[i], measList)
#     print("")
#     time.sleep(0.5)

HDL.streamValues(HDL.users[i],HDL.addrs[i], 90*60)
