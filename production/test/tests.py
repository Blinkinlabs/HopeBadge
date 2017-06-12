#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest
import blinkinlabsunittest
#import colour_runner
#import redgreenunittest as unittest
#import redgreenunittest

import eightbyeight 
import RPi.GPIO as GPIO
import time


class LeoBlinky2016Tests(unittest.TestCase):
    	@classmethod
	def setUpClass(self):
		self.dut = eightbyeight.EightByEightTestRig()

		self.results = {}

    	@classmethod
	def tearDownClass(self):
		self.dut.reset()

		print(self.results)
		print("\n")
		with open("data.log", "a") as logfile:
			logfile.write(str(self.results))
			logfile.write("\n")


	def test_000_initDut(self):
		self.dut.reset()

		# Make sure the radio is disabled?
		self.assertTrue(True)

### Power on tests

#	def test_010_shortTest(self):
#		self.dut.testrig.setPowerMode("limited")
#		IIN_MIN = 30
#		IIN_MAX = 80
#
#		time.sleep(3.5)
#		power = self.dut.testrig.readDutPower()
#		self.results["shortTest_power"] = power
#
#		self.assertGreaterEqual(power["Iin"],IIN_MIN)
#		self.assertLessEqual(power["Iin"],IIN_MAX)
#
#	def test_020_poweronTest(self):
#		self.dut.testrig.setPowerMode("full")
#		IIN_MIN = 30
#		IIN_MAX = 60
#		VIN_MIN = 5
#		VIN_MAX = 5.3
#
#		time.sleep(.5)
#		power = self.dut.testrig.readDutPower()
#		self.results["poweronTest_power"] = power
#
#		self.assertGreaterEqual(power["Iin"],IIN_MIN)
#		self.assertLessEqual(power["Iin"],IIN_MAX)
#		self.assertGreaterEqual(power["Vin"],VIN_MIN)
#		self.assertLessEqual(power["Vin"],VIN_MAX)


# Kinetis programming tests


	def test_400_armEraseFlash(self):
		self.dut.testrig.setPowerMode("full")
		self.dut.testrig.enableUSB()

		self.assertTrue(self.dut.armFlasher.eraseFlash())

	def test_410_armProgramBootloader(self):
		self.dut.testrig.setPowerMode("full")
		self.dut.testrig.enableUSB()

		self.assertTrue(self.dut.armFlasher.writeFirmware("../../bin/blinky-boot-v100.hex"))

	def test_420_usbBootloaderMode(self):
		self.dut.testrig.setPowerMode("full")
		self.dut.testrig.enableUSB()

		self.assertTrue(self.dut.checkForUsbBootloaderDevice())

	def test_430_armProgramFirmware(self):
		self.dut.testrig.setPowerMode("full")
		self.dut.testrig.enableUSB()

		self.assertTrue(self.dut.armFlasher.writeFirmware("../../bin/leoblinky-app-image-v100.hex"))


	def test_440_usbApplicationMode(self):
		self.dut.testrig.setPowerMode("full")
		self.dut.testrig.enableUSB()

		self.assertTrue(self.dut.checkForUsbDevice())

	def test_450_readAcmDeviceInfo(self):

		self.results["readAcmDeviceInfo"] = self.dut.readAcmDeviceInfo()


# LED current tests

# Radio test

# Button test


if __name__ == '__main__':
	import userinterface
	import colorama

	rig = eightbyeight.EightByEightTestRig()

	rig.testrig.setLED("pass", True)
	rig.testrig.setLED("fail", True)

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


		while (not rig.testrig.readStartButton()):
			pass
	
		rig.testrig.setLED("pass", True)
		rig.testrig.setLED("fail", True)

		#runner = unittest.TextTestRunner(failfast = True)
		#runner = redgreenunittest.TextTestRunner(failfast = True)
		runner = blinkinlabsunittest.BlinkinlabsTestRunner(failfast = True)
		result = runner.run(unittest.TestLoader().loadTestsFromTestCase(LeoBlinky2016Tests))

		if len(result.failures) > 0 or len(result.errors) > 0:
			rig.testrig.setLED("pass", False)
			rig.testrig.setLED("fail", True)
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
			rig.testrig.setLED("pass", True)
			rig.testrig.setLED("fail", False)

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
