import subprocess
import time
import sys

def writeFuses(fuse, hfuse, efuse): 
  """
  Attempt to write the fuses of the attached Atmega device.

  """
  command = [
    "avrdude",
    "-C", "+avrdude_gpio.conf", " ",
    "-c", "testrig",
    "-p", "attiny45",
    "-b", "200",
    "-e",
    "-U", "lfuse:w:%#02X:m" % lfuse,
    "-U", "hfuse:w:%#02X:m" % hfuse,
    "-U", "efuse:w:%#02X:m" % efuse,
  ]

  s = open('result.log','w')
  e = open('errorresult.log','w')
  result = subprocess.call(command, stdout=s, stderr=e)
  s.close()
  e.close()

  s = open('result.log','r')
  e = open('errorresult.log','r')
  stdout = s.readlines()
  stderr = e.readlines()
  s.close()
  e.close()

  return result, stdout, stderr

def loadFlash(flashFile):
  """
  Attempt to write a .hex file to the flash of the attached Atmega device.
  @param portName String of the port name to write to
  @param flashFile Array of file(s) to write to the device
  """
  command = [
    "avrdude",
    "-C", "+avrdude_gpio.conf", " ",
    "-c", "testrig",
    "-p", "attiny45",
    "-b", "1",
    "-U" "flash:w:%s:i" % flashFile,
  ]

  s = open('result.log','w')
  e = open('errorresult.log','w')
  result = subprocess.call(command, stdout=s, stderr=e)
  s.close()
  e.close()

  s = open('result.log','r')
  e = open('errorresult.log','r')
  stdout = s.readlines()
  stderr = e.readlines()
  s.close()
  e.close()

  return result, stdout, stderr


if __name__ == '__main__':
  lfuse    = 0x42
  hfuse    = 0xDE
  efuse    = 0xFF

  returnCode = writeFuses(lfuse, hfuse, efuse)

  if (returnCode[0] != 0):
    print "FAIL. Error writing the fuses!"
    print returnCode[1]
    print returnCode[2]
    exit(1)

  print "PASS. Fuses written correctly"

  returnCode = loadFlash("../../bin/hopebadge-v020.hex")

  if (returnCode[0]!= 0):
    print "FAIL. Error programming flash!"
    print returnCode[1]
    print returnCode[2]
    exit(1)

  print "PASS. Flash programmed successfully"

