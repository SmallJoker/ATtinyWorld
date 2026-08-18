import argparse
import io
import math
import serial
import sys
import time

from dataclasses import dataclass

def list_ports():
	import serial.tools.list_ports
	print("Available ports:")
	for p in serial.tools.list_ports.comports():
		print("\t", p)

def dump_bytes(prefix : str, what : bytes):
	print(prefix, end='')
	for b in what:
		if b >= 0x20 and b <= 0x7F:
			print("%02X/%c  " % (b, b), end='')
		else:
			print("%02X  " % b, end='')

	print("." if len(what) == 0 else "")


@dataclass
class Values():
	status : int = 0x00
	vbat : int = 0x00
	th1 : int = 0x00
	th2 : int = 0x00
values = Values()


com : serial.Serial

rcv_bytes : bytes = b''
def receive() -> bytes:
	global rcv_bytes
	buf = com.read(10)
	rcv_bytes += buf
	return buf

def get_payload(req : bytes) -> bytes:
	com.write(req)
	buf = receive()
	if len(buf) < 2:
		return b''
	if buf[0] == ord('I'):
		buf = buf[1 :]
	if buf[0] != ord('>'):
		return b''
	return buf[1 :]

def get_stats():
	payload = get_payload(b'\x81')
	if len(payload) > 0:
		values.status = payload[0]

	payload = get_payload(b'\x90')
	if len(payload) > 0:
		values.vbat = payload[0]

	payload = get_payload(b'\x91')
	if len(payload) > 0:
		values.th1 = payload[0]

	payload = get_payload(b'\x92')
	if len(payload) > 0:
		values.th2 = payload[0]

def status_to_str():
	v = values.status
	return "%c%c%c%c%c%c" % (
			" D"[(v & 0x01) != 0], # discharging
			" I"[(v & 0x02) != 0], # input
			"F "[(v & 0x04) != 0], # TH1 fail
			" C"[(v & 0x10) != 0], # charging
			" O"[(v & 0x20) != 0], # out
			" f"[(v & 0x40) != 0], # fan
		)

def ntc_val_to_celsius(adc : int) -> float:
	"""
	Converts the ADC value [0, 255] to a temperature in °C.
	This is the inverse of firmware/main.c
	"""

	v = float(adc) / 255.0
	# Constants
	B   = 3380        # K, NTC B value
	ZC  =  273.15     # K, 0°C offset
	T_0 =   25.0 + ZC # K, Reference temperature of the sensor
	R_0 = 10E3        # Ohm, Resistance at (T_0)
	R_u = 10E3        # Ohm, Pull-Up resistor

	ln = math.log(R_0 / ((R_u * v) / (1.0 - v)))
	T_x = (B * T_0) / (B - T_0 * ln)
	return T_x - ZC

# Unittest
x = ntc_val_to_celsius(0x80)
assert x > 23 and x < 27

def print_stats():
	print("Status: '%s' (0x%02X),  VBAT %.2f V (0x%02X),  TH1 %.1f °C (0x%02X),  TH2 %.1f °C (0x%02X)" % (
		status_to_str(), values.status,
		(float(values.vbat) - (-2.51)) / 16.41, values.vbat,
		ntc_val_to_celsius(values.th1), values.th1,
		 ntc_val_to_celsius(values.th2), values.th2
	))
	pass


def main():
	if len(sys.argv) < 2:
		print("Error! Use 'python3 monitor.py COMPORT")
		list_ports()
		return
	port_name = sys.argv[1]

	global com
	global rcv_bytes
	try:
		com = serial.Serial(port_name, 38400, serial.EIGHTBITS, serial.PARITY_EVEN, serial.STOPBITS_ONE, 0.02)
	except Exception as e:
		print("Error! %s" % e)
		list_ports()
		return

	commands = {
		"r": "Restart",
		"s": "Sleep"
	}
	print("Commands:")
	for k, v in commands.items():
		print("\t%s : %s" % (k, v))

	print(">> WARNING! TH1 and TH2 might not be sampled! <<")
	while True:
		buf = receive()
		do_sleep = False
		if len(buf) > 0:
			get_stats()
			print_stats()
			do_sleep = True
		#dump_bytes("", rcv_bytes)
		rcv_bytes = b''
		if do_sleep:
			time.sleep(3)

		# TODO: Handle commands


if __name__ == '__main__':
	main()
