import serial

arduino = serial.Serial(port='COM3',  baudrate=9600, timeout=.1)
while True:
    data = arduino.readline()
    clean = data.strip(b'\xff\xdf')
    txt = clean.decode()
    print(txt, end='')
