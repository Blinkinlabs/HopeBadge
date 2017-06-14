#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import blinkinlabsunittest

import avrdude
import testrig

import RPi.GPIO as GPIO
import time

import os.path

class HopeBadgeTests(unittest.TestCase):
    reset_pin = "JTAG_RESET"
    led_1_pin = "JTAG_TDI"
    led_2_pin = "JTAG_TDO"
    led_3_pin = "2"
#   ir_en_pin = "JTAG_TCK"
#   ir_data_pin = "1"

    @classmethod
    def setUpClass(self):
        self.results = {}

        # Pull the reset line low
        rig.digitalPinMode(self.reset_pin, GPIO.OUT)
        rig.digitalPinWrite(self.reset_pin, GPIO.LOW)

        rig.digitalPinMode(self.led_1_pin, GPIO.OUT)
        rig.digitalPinWrite(self.led_1_pin, GPIO.HIGH)

        rig.digitalPinMode(self.led_2_pin, GPIO.OUT)
        rig.digitalPinWrite(self.led_2_pin, GPIO.HIGH)

        rig.digitalPinMode(self.led_3_pin, GPIO.OUT)
        rig.digitalPinWrite(self.led_3_pin, GPIO.HIGH)

#       rig.digitalPinMode(self.ir_en_pin, GPIO.OUT)
#       rig.digitalPinWrite(self.ir_en_pin, GPIO.LOW)

    @classmethod
    def tearDownClass(self):
        rig.setPowerMode("off")

        rig.digitalPinMode(self.reset_pin, GPIO.IN)
        rig.digitalPinMode(self.led_1_pin, GPIO.IN)
        rig.digitalPinMode(self.led_2_pin, GPIO.IN)
        rig.digitalPinMode(self.led_3_pin, GPIO.IN)
#       rig.digitalPinMode(self.ir_en_pin, GPIO.IN)

        # print(self.results)

        for key, value in self.results.items():
            print(key),
            print(value)

        print("\n")
        with open("data.log", "a") as logfile:
            logfile.write(str(self.results))
            logfile.write("\n")


# Power on tests
    def test_010_short(self):
        rig.setPowerMode("limited")
        IIN_MIN = -1
        IIN_MAX = 1
        VIN_MIN = 3.1
        VIN_MAX = 3.5

        power = rig.readDutPower()
        self.results["shortTest_power"] = power

        self.assertGreaterEqual(power["I"], IIN_MIN)
        self.assertLessEqual(power["I"], IIN_MAX)
        self.assertGreaterEqual(power["Vbus"], VIN_MIN)
        self.assertLessEqual(power["Vbus"], VIN_MAX)

    def test_020_powerOn(self):
        rig.setPowerMode("full")
        IIN_MIN = -1
        IIN_MAX = 1
        VIN_MIN = 3.1
        VIN_MAX = 3.5

        power = rig.readDutPower()
        self.results["poweronTest_power"] = power

        self.assertGreaterEqual(power["I"], IIN_MIN)
        self.assertLessEqual(power["I"], IIN_MAX)
        self.assertGreaterEqual(power["Vbus"], VIN_MIN)
        self.assertLessEqual(power["Vbus"], VIN_MAX)

# Peripheral power draw test

    def test_100_led1(self):
        IIN_MIN = 9
        IIN_MAX = 14

        rig.digitalPinWrite(self.led_1_pin, GPIO.LOW)

        power = rig.readDutPower()

        rig.digitalPinWrite(self.led_1_pin, GPIO.HIGH)

        self.results["led1Test_power"] = power
        self.assertGreaterEqual(power["I"], IIN_MIN)
        self.assertLessEqual(power["I"], IIN_MAX)

    def test_110_led2(self):
        IIN_MIN = 9
        IIN_MAX = 14

        rig.digitalPinWrite(self.led_2_pin, GPIO.LOW)

        power = rig.readDutPower()

        rig.digitalPinWrite(self.led_2_pin, GPIO.HIGH)

        self.results["led2Test_power"] = power
        self.assertGreaterEqual(power["I"], IIN_MIN)
        self.assertLessEqual(power["I"], IIN_MAX)

    def test_120_led3(self):
        IIN_MIN = 9
        IIN_MAX = 14

        rig.digitalPinWrite(self.led_3_pin, GPIO.LOW)

        power = rig.readDutPower()

        rig.digitalPinWrite(self.led_3_pin, GPIO.HIGH)

        self.results["led3Test_power"] = power
        self.assertGreaterEqual(power["I"], IIN_MIN)
        self.assertLessEqual(power["I"], IIN_MAX)

#   def test_130_irReceiver(self):
#       IIN_MIN = -1
#       IIN_MAX = 3
#
#       rig.digitalPinWrite(self.ir_en_pin, GPIO.HIGH)
#
#       power = rig.readDutPower()
#
#       rig.digitalPinWrite(self.ir_en_pin, GPIO.LOW)
#
#       self.results["irReceiver_power"] = power
#       self.assertGreaterEqual(power["I"],IIN_MIN)
#       self.assertLessEqual(power["I"],IIN_MAX)


# ICSP tests
    def test_200_clearExports(self):
        """
        Sometimes avrdude might mess up and leave some gpio
        pins mapped, so let's unexport them to be safe.
        """
        pins = ["7", "9", "10", "11"]

        for pin in pins:
            if os.path.isdir("/sys/class/gpio/gpio" + pin):
                with open("/sys/class/gpio/unexport", "w") as f:
                    f.write(pin)

    def test_210_writeFuses(self):
        """
        Write the fuses, using a slow ICSP communication speed
        """
        lfuse = 0x42
        hfuse = 0xDE
        efuse = 0xFF

        results = avrdude.writeFuses(lfuse, hfuse, efuse)
        self.results["writeFusesTest_result"]=results

        self.assertEqual(results[0], 0)

    def test_220_programFirmware(self):
        """
        Program the firmware, using a faster ICSP communication speed
        """
        firmware = "../../bin/hopebadge-v030.hex"

        results = avrdude.loadFlash(firmware)
        self.results["programFirwareTest_result"]=results

        self.assertEqual(results[0], 0)


if __name__ == '__main__':
    import userinterface
    import colorama

    rig = testrig.TestRig()

    rig.setLED("pass", True)
    rig.setLED("fail", True)

    while True:
        message = """
  _____  ______          _______     __
 |  __ \|  ____|   /\   |  __ \ \   / /
 | |__) | |__     /  \  | |  | \ \_/ /
 |  _  /|  __|   / /\ \ | |  | |\   /
 | | \ \| |____ / ____ \| |__| | | |
 |_|  \_\______/_/    \_\_____/  |_|
"""
        userinterface.interface.DisplayMessage(
            message, fgcolor=colorama.Fore.BLUE)

        while (not rig.readStartButton()):
            pass

        rig.setLED("pass", True)
        rig.setLED("fail", True)

        runner = blinkinlabsunittest.BlinkinlabsTestRunner(failfast=True)
        result = runner.run(
            unittest.TestLoader().loadTestsFromTestCase(HopeBadgeTests))

        if len(result.failures) > 0 or len(result.errors) > 0:
            rig.setLED("pass", False)
            rig.setLED("fail", True)
            message = """
  ______      _____ _
 |  ____/\   |_   _| |
 | |__ /  \    | | | |
 |  __/ /\ \   | | | |
 | | / ____ \ _| |_| |____
 |_|/_/    \_\_____|______|
"""
            userinterface.interface.DisplayMessage(
                message, fgcolor=colorama.Fore.BLACK, bgcolor=colorama.Back.RED)

        else:
            rig.setLED("pass", True)
            rig.setLED("fail", False)

            message = """
  ____     _   __
 / __ \   | | / /
| |  | |  | |/ /
| |  | |  |   |
| |__| |  | |\ \\
 \____/   |_| \_\\
"""
            userinterface.interface.DisplayMessage(
                message, fgcolor=colorama.Fore.BLACK, bgcolor=colorama.Back.GREEN)

        time.sleep(1)
