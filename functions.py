import serial
from pynput.keyboard import Key, Controller
Serial = serial.Serial('/dev/ttyUSB0', 9600)
Keyboard = Controller()

def reading():
    resp: str = Serial.readline().decode("utf-8")
    return resp[:-3]

def writing(clear, word):
    if clear:
        with Keyboard.pressed(Key.ctrl):
            Keyboard.tap(Key.backspace)
    Keyboard.type(word.replace("д", "d").replace())

#Serial.close()