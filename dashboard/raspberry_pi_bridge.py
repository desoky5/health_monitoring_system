#!/usr/bin/env python3
"""Forward STM32 health measurements to the hosted dashboard API."""

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
DATA_PATTERN = re.compile(
    r"\bDATA\s*:\s*BPM\s*=\s*(\d+(?:\.\d+)?)\s*,\s*"
    r"STEPS\s*=\s*(\d+(?:\.\d+)?)\s*,\s*"
    r"ACC\s*=\s*(\d+(?:\.\d+)?)\b",
    re.IGNORECASE,
)


def publish_measurements(measurements):
    payload = json.dumps(measurements).encode("utf-8")
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
            data_match = DATA_PATTERN.search(line)
            if data_match:
                measurements = {
                    "heartRate": float(data_match.group(1)),
                    "steps": float(data_match.group(2)),
                    "acceleration": float(data_match.group(3)),
                }
            else:
                bpm_match = BPM_PATTERN.search(line)
                if not bpm_match:
                    continue
                measurements = {"heartRate": float(bpm_match.group(1))}

            for key, value in list(measurements.items()):
                if value.is_integer():
                    measurements[key] = int(value)

            try:
                result = publish_measurements(measurements)
                print(f"Dashboard: {result}")
            except (OSError, urllib.error.URLError, RuntimeError) as error:
                print(f"Dashboard update failed: {error}")
                time.sleep(2)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Bridge stopped")
