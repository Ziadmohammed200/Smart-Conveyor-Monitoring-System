# Smart Conveyor Monitoring & Control System (Proteus Simulation)

This project simulates a **smart conveyor system** using an ARM Cortex-M microcontroller.  
It combines motor control, speed measurement, object detection, and safety features in a fully integrated simulation.

---

## 📝 Project Overview

The system:

- Measures **conveyor speed** using **Timer Capture** and a simulated function generator.
- Controls **motor speed** via **PWM** based on a potentiometer input.
- Detects passing objects using **software-based edge detection**.
- Includes an **emergency stop** triggered by an external interrupt.
- Displays real-time information on a **16x2 LCD**:
  - Conveyor speed
  - Motor speed (as % PWM)
  - Object count
  - Emergency status

---

## 🛠️ System Requirements & Constraints

1. **Speed Measurement**
   - Timer Input Capture mode, no interrupts for edge detection.
   - Function generator simulates the pulse signal.

2. **Motor Speed Control**
   - Potentiometer connected to ADC channel.
   - PWM output regulates motor speed.

3. **Object Detection**
   - IR sensor simulated as digital input.
   - Falling-edge detection implemented in software (polling).

4. **Emergency Stop**
   - Push button triggers an external interrupt.
   - Motor stops and LCD displays "EMERGENCY STOP".

5. **Display Interface**
   - 16x2 LCD displays all relevant system data.

---

## 📂 Deliverables
# Contact me for the whole Project 
- **Proteus Simulation File (.pdsprj)** with all subsystems integrated.
- **C Source Code**, modular and well-documented.
- **Brief Report (PDF)**:
  - Project overview
  - Block diagram
  - Subsystem explanations
  - Simulation screenshots

---

## 🖼️ Block Diagram
<img width="1920" height="1080" alt="Screenshot 2025-07-28 225419" src="https://github.com/user-attachments/assets/46e23da0-5754-481e-9fb7-ea8f285db8f2" />
<img width="1920" height="1080" alt="Screenshot 2025-07-28 225338" src="https://github.com/user-attachments/assets/a13688d6-e4a7-42c6-8b87-4b8ec6e4035b" />



