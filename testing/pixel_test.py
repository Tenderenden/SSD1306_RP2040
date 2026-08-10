import time
import serial
# import serial_connection as sc

SCREEN_WIDTH = 128
SCREEN_HEIGHT = 32

sc = serial.Serial('/dev/ttyUSB0', 115200)

def pixel_on(x, y):
    print("Pixel {},{} on".format(x, y))
    sc.write(bytes("p {} {} 1".format(x, y), 'utf-8'))

def pixel_off(x, y):
    print("Pixel {},{} off".format(x, y))
    sc.write(bytes("p {} {} 0".format(x, y), 'utf-8'))

#Turn every pixel on and off
if __name__ == "__main__":
    for y in range(SCREEN_HEIGHT):
        for x in range(SCREEN_WIDTH):
            pixel_on(x, y)
            time.sleep(0.5)
            # pixel_off(x, y)
            # time.sleep(2)