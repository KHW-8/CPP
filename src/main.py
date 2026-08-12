import serial

with serial.Serial(port="COM3", baudrate=115200, timeout=1) as ser:
    while True:
        msg = ser.read_until()
        if (len(msg) == 0):
            continue

        print(f"Received bytes: {len(msg)}, Message: {msg}")