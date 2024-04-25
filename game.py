import serial
from pynput.keyboard import Key, Controller
from time import sleep
Serial = serial.Serial('/dev/ttyUSB0', 9600)
Keyboard = Controller()

def reading():
    resp: str = Serial.readline().decode("utf-8")
    return resp[:-3]

sleep(5)

buttons = ["W", "A", "S", "D", Key.space, Key.ctrl_l, Key.shift_l, "Q", "E"]
old_presses = ["0"] * len(buttons)
while True:
    read = reading()
    presses = [read[1], read[2], read[0], read[7], read[3], read[11], read[9], read[6], read[8]]
    for i in range(9):
        if presses[i] != old_presses[i]:
            match presses[i]:
                case "1":
                    Keyboard.press(buttons[i])
                case "0":
                    Keyboard.release(buttons[i])
    old_presses = presses
