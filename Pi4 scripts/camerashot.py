import time
from picamera import PiCamera



camera = PiCamera()
camera.resolution = (1024,768)
print('Start Camera')
camera.start_preview()
time.sleep(3)
camera.capture(f'{time.time()}.jpg')
