import RPi.GPIO as GPIO


class Pins:
    def __init__(self):
        self.__start_pin = 13
        self.__stop_pin = 14

    def set(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(self.__start_pin, GPIO.IN)
        GPIO.input(self.__stop_pin, 1)

    def status(self):
        if GPIO.input(self.__start_pin):
            stat = 'on'
        if GPIO.input(self.__stop_pin):
            stat = 'off'
        else:
            stat = None

        return stat
