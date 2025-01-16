
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = pd.read_csv('moisture_sensor_output.txt', delim_whitespace=True, skiprows=1, 
                   names=['step', 'voltage', 'from', 'to'])

capacitance = np.linspace(40, 300, 15)

plt.figure(figsize=(10, 6))
plt.plot(capacitance, data['voltage'], '-o')

plt.xlabel('Capacitance (pF)')
plt.ylabel('Output Voltage (V)')
plt.title('Output Voltage vs Capacitance')
plt.grid(True)

plt.grid(True, which='minor', linestyle='--', alpha=0.4)
plt.minorticks_on()

plt.savefig('voltage_vs_capacitance.pdf', bbox_inches='tight', dpi=300)
plt.show()
