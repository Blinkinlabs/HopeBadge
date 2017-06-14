#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import blinkinlabsunittest
#import colour_runner
#import redgreenunittest as unittest
#import redgreenunittest

import avrdude
import testrig

import RPi.GPIO as GPIO
import time



class HopeBadgeTests(unittest.TestCase):
	reset_pin = "JTAG_RESET"
        led_1_pin = "JTAG_TDI"
        led_2_pin = "JTAG_TDO"
#	led_3_pin = ""
	ir_en_pin = "JTAG_TCK"

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

#		rig.digitalPinMode(self.led_3_pin, GPIO.OUT)
#		rig.digitalPinWrite(self.led_3_pin, GPIO.HIGH)

		rig.digitalPinMode(self.ir_en_pin, GPIO.OUT)
		rig.digitalPinWrite(self.ir_en_pin, GPIO.LOW)

    	@classmethod
	def tearDownClass(self):
		rig.setPowerMode("off")

		rig.digitalPinMode(self.reset_pin, GPIO.IN)
		rig.digitalPinMode(self.led_1_pin, GPIO.IN)
		rig.digitalPinMode(self.led_2_pin, GPIO.IN)
#		rig.digitalPinMode(self.led_3_pin, GPIO.IN)
		rig.digitalPinMode(self.ir_en_pin, GPIO.IN)

		print(self.results)
		print("\n")
		with open("data.log", "a") as logfile:
			logfile.write(str(self.results))
			logfile.write("\n")


### Power on tests
	def test_010_shortTest(self):
		# TODO: Pull reset low

		rig.setPowerMode("limited")
		IIN_MIN = -1
		IIN_MAX = 1
		VIN_MIN = 3.2
		VIN_MAX = 3.4

		power = rig.readDutPower()
		print(power)
		self.results["shortTest_power"] = power

		self.assertGreaterEqual(power["I"],IIN_MIN)
		self.assertLessEqual(power["I"],IIN_MAX)
		self.assertGreaterEqual(power["Vbus"],VIN_MIN)
		self.assertLessEqual(power["Vbus"],VIN_MAX)

	def test_020_poweronTest(self):
		# TODO: Pull reset low
		rig.setPowerMode("full")
		IIN_MIN = -1
		IIN_MAX = 1
		VIN_MIN = 3.2
		VIN_MAX = 3.4

		power = rig.readDutPower()
		self.results["poweronTest_power"] = power

		self.assertGreaterEqual(power["I"],IIN_MIN)
		self.assertLessEqual(power["I"],IIN_MAX)
		self.assertGreaterEqual(power["Vbus"],VIN_MIN)
		self.assertLessEqual(power["Vbus"],VIN_MAX)

# Peripheral power draw test

	def test_100_led1Test(self):
		IIN_MIN = 10
		IIN_MAX = 14

		rig.digitalPinWrite(self.led_1_pin, GPIO.LOW)

		power = rig.readDutPower()

		rig.digitalPinWrite(self.led_1_pin, GPIO.HIGH)

		self.results["led1Test_power"] = power
		self.assertGreaterEqual(power["I"],IIN_MIN)
		self.assertLessEqual(power["I"],IIN_MAX)

	def test_110_led2Test(self):
		IIN_MIN = 10
		IIN_MAX = 14

		rig.digitalPinWrite(self.led_2_pin, GPIO.LOW)

		power = rig.readDutPower()

		rig.digitalPinWrite(self.led_2_pin, GPIO.HIGH)

		self.results["led2Test_power"] = power
		self.assertGreaterEqual(power["I"],IIN_MIN)
		self.assertLessEqual(power["I"],IIN_MAX)

# TODO: Wire this LED so it can be controlled
#	def test_120_led3Test(self):
#		IIN_MIN = 10
#		IIN_MAX = 14
#
#		rig.digitalPinWrite(self.led_3_pin, GPIO.LOW)
#
#		power = rig.readDutPower()
#
#		rig.digitalPinWrite(self.led_3_pin, GPIO.HIGH)
#
#		self.results["led3Test_power"] = power
#		self.assertGreaterEqual(power["I"],IIN_MIN)
#		self.assertLessEqual(power["I"],IIN_MAX)

	def test_130_irReceiverTest(self):
		IIN_MIN = -1
		IIN_MAX = 3

		rig.digitalPinWrite(self.ir_en_pin, GPIO.HIGH)

		power = rig.readDutPower()

		rig.digitalPinWrite(self.ir_en_pin, GPIO.LOW)

		self.results["irReceiver_power"] = power
		self.assertGreaterEqual(power["I"],IIN_MIN)
		self.assertLessEqual(power["I"],IIN_MAX)


# ICSP tests
	def test_200_writeFuses(self):
  		lfuse    = 0x42
		hfuse    = 0xDE
		efuse    = 0xFF

		returnCode = avrdude.writeFuses(lfuse, hfuse, efuse)
		self.assertEqual(returnCode[0], 0)

	def test_210_programFuses(self):
                firmware = "../../bin/hopebadge-v030.hex"

		returnCode = avrdude.loadFlash(firmware)
		self.assertEqual(returnCode[0], 0)



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
		userinterface.interface.DisplayMessage(message, fgcolor=colorama.Fore.BLUE)


		while (not rig.readStartButton()):
			pass
	
		rig.setLED("pass", True)
		rig.setLED("fail", True)

		#runner = unittest.TextTestRunner(failfast = True)
		#runner = redgreenunittest.TextTestRunner(failfast = True)
		runner = blinkinlabsunittest.BlinkinlabsTestRunner(failfast = True)
		result = runner.run(unittest.TestLoader().loadTestsFromTestCase(HopeBadgeTests))

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
			userinterface.interface.DisplayMessage(message, fgcolor=colorama.Fore.BLACK, bgcolor=colorama.Back.RED)

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
			userinterface.interface.DisplayMessage(message, fgcolor=colorama.Fore.BLACK, bgcolor=colorama.Back.GREEN)

		time.sleep(1)
