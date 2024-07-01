import serial
import serial.tools
import serial.tools.list_ports
import time

OPEN_CMD = b'\xA0\x01\x01\xA2'
CLOSE_CMD = b'\xA0\x01\x00\xA1'

target_ports = []
while True:
    del target_ports[:]
    comports = serial.tools.list_ports.comports()

    for port in comports:
        if 'VID:PID=1A86:7523' in port.hwid:
            target_ports.append(port)
    
    for port in target_ports:
        try:
            print('Opening Port: {}'.format(port.name))
            ser = serial.Serial(port.name)        
            ser.write(OPEN_CMD)
            time.sleep(0.05)
            ser.write(CLOSE_CMD)
            time.sleep(0.05)
            print('Closing Port')
            ser.close()
        except Exception as ex:
            print(ex)
       


    
    
    
    
    
    