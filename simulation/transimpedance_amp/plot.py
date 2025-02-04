# Bode plot for transimpedance amplifier
# Photodiode reverse current 1 to 100 uA -> 0 to 3.3v output
import matplotlib.pyplot as plt
import numpy as np

Rf = 33e3# 
Cf = 1e-6   # 1nF
Id = 100e-6 # 100uA reverse diode current

f = np.logspace(0, 6, 1000)  # 1 Hz to 1 MHz
w = 2 * np.pi * f            # Angular frequency
s = 1j * w                   # Complex frequency

def transfer_function(R, C, s):
    return R / (R*C*s + 1)

magnitude_db = 20 * np.log10(np.abs(transfer_function(Rf, Cf, s)))
voltage_out = np.abs(transfer_function(Rf, Cf, s)) * Id

# Create figure with two subplots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))

# Bode plot
ax1.semilogx(f, magnitude_db)
ax1.grid(True)
ax1.set_xlabel('Frequency (Hz)')
ax1.set_ylabel('Magnitude (dB)')
ax1.set_title('Bode Plot of Transimpedance Amplifier')

# Voltage output plot
ax2.semilogx(f, voltage_out)
ax2.grid(True)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Output Voltage (V)')
ax2.set_title(f'Output Voltage at {Id*1000:.0f}mA Input Current')

plt.tight_layout()
plt.show()
