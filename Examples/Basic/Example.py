from Mania_Telemetry import TrackmaniaTelemetry
import time

TM = TrackmaniaTelemetry()

while True:
    y = TM.get()
    print("Speed: {}, Steering: {}, Gas: {}, Break: {}".format(y.SpeedMeter, y.InputSteer, y.InputGasPedal, y.InputIsBraking))
    time.sleep(0.1)