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
    	@classmethod
	def setUpClass(self):
		self.results = {}

    	@classmethod
	def tearDownClass(self):
		print(self.results)
		print("\n")
		with open("data.log", "a") as logfile:
			logfile.write(str(self.results))
			logfile.write("\n")


#	def test_000_initDut(self):
#		self.dut.reset()
#
#		# Make sure the radio is disabled?
#		self.assertTrue(True)

### Power on tests

#	def test_010_shortTest(self):
#		self.testrig.setPowerMode("limited")
#		IIN_MIN = 0
#		IIN_MAX = 20
#
#		time.sleep(3.5)
#		power = self.dut.testrig.readDutPower()
#		self.results["shortTest_power"] = power
#
#		self.assertGreaterEqual(power["Iin"],IIN_MIN)
#		self.assertLessEqual(power["Iin"],IIN_MAX)
#
#	def test_020_poweronTest(self):
#		self.testrig.setPowerMode("full")
#		IIN_MIN = 0
#		IIN_MAX = 20
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


	def test_100_writeFuses(self):
  		lfuse    = 0x42
		hfuse    = 0xDE
		efuse    = 0xFF

		returnCode = avrdude.writeFuses(lfuse, hfuse, efuse)
		self.assertEqual(returnCode[0], 0)

	def test_110_programFuses(self):
                firmware = "../../bin/hopebadge-v030.hex"

		returnCode = avrdude.loadFlash(firmware)
		self.assertEqual(returnCode[0], 0)

if __name__ == '__main__':
	import userinterface
	import colorama

	testrig = testrig.TestRig()

	testrig.setLED("pass", True)
	testrig.setLED("fail", True)

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


		while (not testrig.readStartButton()):
			pass
	
		testrig.setLED("pass", True)
		testrig.setLED("fail", True)

		#runner = unittest.TextTestRunner(failfast = True)
		#runner = redgreenunittest.TextTestRunner(failfast = True)
		runner = blinkinlabsunittest.BlinkinlabsTestRunner(failfast = True)
		result = runner.run(unittest.TestLoader().loadTestsFromTestCase(HopeBadgeTests))

		if len(result.failures) > 0 or len(result.errors) > 0:
			testrig.setLED("pass", False)
			testrig.setLED("fail", True)
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
			testrig.setLED("pass", True)
			testrig.setLED("fail", False)

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
