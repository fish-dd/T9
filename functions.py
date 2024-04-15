import serial
import keyboard
Serial = serial.Serial('/dev/ttyUSB0', 9600)

def reading():
    resp: str = Serial.readline().decode("utf-8")
    return resp[:-3]

def writing(clear, word):
    if clear:
        keyboard.send("ctrl+backspace")
    keyboard.write(word)

#Serial.close()