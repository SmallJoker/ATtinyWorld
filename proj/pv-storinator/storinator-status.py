# Script to communicate with the pv-storinator MCU
# SPDX-License-Identifier: MIT


from __future__ import annotations

import io
import re
import serial
import sys

def dbgprint(*args, **kwargs):
	print(*args, file=sys.stderr, **kwargs)

def error(code: int, *args, **kwargs):
	dbgprint(*args, **kwargs)
	sys.exit(code)

class Storinator:
	con : serial.Serial = None
	ADC_VBAT_PERC = [ 10, 40, 90 ] # approx. percentages
	ADC_VBAT_VAL = []
	curve = []

	def __init__(self, port: str):
		re_vbat = re.compile("^\s+ADC_VBAT_\w+\s+=\s+([x0-9A-Fa-f]+).*$")

		for i, line in enumerate(io.open("firmware/main.c", "r")):
			match = re.match(re_vbat, line)
			if not match:
				continue

			# Automatic base detection
			v = int(match.group(1), 0)
			self.ADC_VBAT_VAL.append(v)

		X = self.ADC_VBAT_VAL
		Y = self.ADC_VBAT_PERC
		dbgprint(f"Battery curve: {Y[0]} % = {X[0]}, {Y[1]} % = {X[1]}, {Y[2]} % = {X[2]}")

		dxQ_21 = X[1]**2 - X[0]**2
		dxQ_32 = X[2]**2 - X[1]**2
		b = (
			(dxQ_32 * (Y[1] - Y[0]) - dxQ_21 * (Y[2] - Y[1]))
				/
			(dxQ_32 * (X[1] - X[0]) - dxQ_21 * (X[2] - X[1]))
		)
		a = (
			((Y[1] - Y[0]) - b * (X[1] - X[0]))
			/
			dxQ_21
		)
		c = Y[1] - a * X[1]**2 - b * X[1]
		self.curve = [a, b, c]

		self.con = serial.Serial(port, 38400, serial.EIGHTBITS, serial.PARITY_EVEN, serial.STOPBITS_ONE, 0.1);
		pass

	def getVBAT(self):
		self.con.write(b'\x90')
		data = self.con.read(3) # b'I>\x00'
		i = data.find(b'>')
		if not i or len(data) - i < 2:
			dbgprint("Insufficient data")
			return

		x = int(data[i + 1])
		# Calculate to percent. +1/-1 makes a difference of 6-7%.
		k = self.curve
		percent = k[0] * x**2 + k[1] * x + k[2]
		dbgprint(f"Battery: {percent:2.0f} % (raw: {x})")
		return int(percent + 0.5)



if __name__ == '__main__':
	if len(sys.argv) < 2:
		error(2, "Syntax: <script> SERIAL_PORT")

	pvs = Storinator(sys.argv[1])
	print(pvs.getVBAT())
