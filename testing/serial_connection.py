import time
import serial

# 115200
if __name__ == "__main__":
    conn = serial.Serial('/dev/ttyUSB0', 115200)
    print(conn.name)
    while(1):
        conn.write(b"test")
        time.sleep(2)
