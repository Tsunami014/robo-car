import serial, os, time, subprocess
from threading import Thread

os.system("rm /tmp/ttyBLE") # If it wasn't removed before automatically, remove it now.
Thread(target=lambda: subprocess.check_output(["ble-serial -d 48:87:2D:6F:9F:9B -t 10"], shell=True, text=True), daemon=True).start()
#p = subprocess.Popen(["ble-serial", "-d", "48:87:2D:6F:9F:9B", "-t", "10"], stdout=subprocess.PIPE, text=True)

while True:
    try:
        arduino = serial.Serial(port='/tmp/ttyBLE', baudrate=9600)
        break
    except:
        time.sleep(1)

def inputLoop():
    while True:
        arduino.write(bytes(input(), 'utf8'))

Thread(target=inputLoop, daemon=True).start()

while True:
    data = arduino.read_all()
    if data is not None:
        clean = data.strip(b'\xff\xdf')
        txt = clean.decode()
        print(txt, end='')
