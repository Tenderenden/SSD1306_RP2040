import time

SCREEN_WIDTH = 128
SCREEN_HEIGHT = 32

def pixel_on(x, y):
    print("Pixel {},{} on".format(x, y))

def pixel_off(x, y):
    print("Pixel {},{} off".format(x, y))

#Turn every pixel on and off
if __name__ == "__main__":
    for y in range(SCREEN_HEIGHT):
        for x in range(SCREEN_WIDTH):
            pixel_on(x, y)
            time.sleep(3)
            pixel_off(x, y)
            time.sleep(2)