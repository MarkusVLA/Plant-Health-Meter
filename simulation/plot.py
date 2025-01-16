# from matplotlib import pyplot as plt
# import pandas as pd
# import numpy as np
#
# sim = pd.read_csv('moisture_sensor.txt', sep='\t') 
# sim2 = pd.read_csv('moisture_sensor_50p.txt', sep='\t')
#
# plt.plot(sim['time'], sim['V(out)'], label = '300pF soil capacitance')
# plt.plot(sim2['time'], sim2['V(out)'], label='50pF soil capacitace')
#
# plt.legend()
#
# plt.xlabel("Time (ms)")
# plt.ylabel("Voltage (V)")
# plt.title("Output voltage")
# plt.show()

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the space-separated data, skipping the header line
data = pd.read_csv('moisture_sensor_output.txt', delim_whitespace=True, skiprows=1, 
                   names=['step', 'voltage', 'from', 'to'])

# Create capacitance values array (from 40pF to 300pF in steps of 20pF)
capacitance = np.linspace(40, 300, 15)

# Create plot
plt.figure(figsize=(10, 6))
plt.plot(capacitance, data['voltage'], '-o')

plt.xlabel('Capacitance (pF)')
plt.ylabel('Output Voltage (V)')
plt.title('Output Voltage vs Capacitance')
plt.grid(True)

# Add minor gridlines
plt.grid(True, which='minor', linestyle='--', alpha=0.4)
plt.minorticks_on()

# Save the plot
plt.savefig('voltage_vs_capacitance.pdf', bbox_inches='tight', dpi=300)
plt.show()
