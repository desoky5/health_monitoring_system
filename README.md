# Portable Health Monitoring Device with Wireless Data Transmission

Graduation project for the ITI ARM (STM32F401) Embedded Systems Diploma. A portable device that monitors heart rate and physical activity, presents the data on an on-device dashboard, and streams it wirelessly to a live web monitoring dashboard.

## Overview

The device is built around the STM32F401 microcontroller. It captures heart-rate and motion data from onboard sensors, gives the user real-time feedback (display, audio, and LED alerts), and transmits health data over Wi-Fi for remote monitoring through a companion web dashboard. Full functional and non-functional requirements are documented in [`docs/Health_Monitor_Requirements_Analysis.pdf`](docs/Health_Monitor_Requirements_Analysis.pdf).

**Live web dashboard:** [Health Monitor](http://healthmonitoringembedded.atwebpages.com/)

## Implemented so far

- **Web monitoring dashboard** (FR-8) — live view of heart rate, step count, and movement/activity status, with a heart-rate trend chart, configurable daily step goal, and light/dark theme
- **Backend API** — receives and serves heart-rate, step, and acceleration readings (PHP endpoint with JSON storage; a Node.js/SSE-based backend is also included as an alternative)
- **Firmware** — driver and feature-logic development in progress per the module breakdown below

## Team & Task Breakdown

| Member | Responsibilities |
|---|---|
| **Omar** | Heart Rate Monitoring module (sensor driver + alert/logging logic), Interactive UI (TFT/7-segment dashboard, settings, history), and the **web monitoring dashboard** (frontend + backend API) |
| **Khaled Gado** | Step Counter & Activity Tracking module (accelerometer driver + step/activity-recognition logic), LED Matrix visualization |
| **Hussien Ahmed** | Wireless Data Transmission (Wi-Fi + secure sync), Audio Feedback & Alerts, Power Management & Manual Controls |

## Documentation

See [`docs/Health_Monitor_Requirements_Analysis.pdf`](docs/Health_Monitor_Requirements_Analysis.pdf) for the full requirements analysis, including detailed functional/non-functional requirements and the project timeline.

---
*ARM Embedded Systems Diploma — ITI, in partnership with IMT School (a Coretech company).*
