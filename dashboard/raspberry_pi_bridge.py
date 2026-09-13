#!/usr/bin/env python3
"""Forward STM32 heart-rate UART messages to the hosted dashboard API."""

import json
import re
import time
import urllib.error
import urllib.request

import serial

SERIAL_PORT = "/dev/serial0"
BAUD_RATE = 9600
API_URL = "http://healthmonitoringembedded.atwebpages.com/api.php"
REQUEST_TIMEOUT_SECONDS = 10

BPM_PATTERN = re.compile(r"\bBPM\s*:\s*(\d+(?:\.\d+)?)\b", re.IGNORECASE)


def publish_heart_rate(heart_rate):
    payload = json.dumps({"heartRate": heart_rate}).encode("utf-8")
    request = urllib.request.Request(
        API_URL,
        data=payload,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    with urllib.request.urlopen(request, timeout=REQUEST_TIMEOUT_SECONDS) as response:
        if response.status < 200 or response.status >= 300:
            raise RuntimeError(f"API returned HTTP {response.status}")
        return response.read().decode("utf-8")


def main():
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud")
    print(f"Publishing heart rate to {API_URL}")

    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as uart:
        while True:
            raw_line = uart.readline()
            if not raw_line:
                continue

            line = raw_line.decode("ascii", errors="replace").strip()
            print(f"STM32: {line}")
            match = BPM_PATTERN.search(line)
            if not match:
                continue

            heart_rate = float(match.group(1))
            if heart_rate.is_integer():
                heart_rate = int(heart_rate)

            try:
                result = publish_heart_rate(heart_rate)
                print(f"Dashboard: {result}")
            except (OSError, urllib.error.URLError, RuntimeError) as error:
                print(f"Dashboard update failed: {error}")
                time.sleep(2)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Bridge stopped")
