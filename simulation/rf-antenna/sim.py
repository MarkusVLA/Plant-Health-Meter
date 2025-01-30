import numpy as np
import matplotlib.pyplot as plt

def calculate_s_parameters(f, C1, L1, C2, Z_source, Z_load=50):
    """
    Calculate S-parameters for pi matching network across frequencies
    
    Args:
        f: Frequency array in Hz
        C1, L1, C2: Component values in Farads and Henries
        Z_source: Source impedance (complex)
        Z_load: Load impedance (default 50 ohms)
    """
    w = 2 * np.pi * f
    s11_array = []
    s21_array = []
    
    for w_i in w:
        Z_C1 = -1j / (w_i * C1)
        Z_L = 1j * w_i * L1
        Z_C2 = -1j / (w_i * C2)
        
        # Calculate ABCD parameters for each component
        # Shunt C1
        A1 = np.array([[1, 0], 
                      [1/Z_C1, 1]])
        
        # Series L
        A2 = np.array([[1, Z_L], 
                      [0, 1]])
        
        # Shunt C2
        A3 = np.array([[1, 0], 
                      [1/Z_C2, 1]])
        
        # Multiply matrices
        A = A1 @ A2 @ A3
        
        # Convert ABCD to S parameters
        # For 50 ohm reference impedance
        A_11 = A[0,0]
        A_12 = A[0,1]
        A_21 = A[1,0]
        A_22 = A[1,1]
        
        denom = A_11 + A_12/Z_load + Z_source*A_21 + Z_source*A_22/Z_load
        
        s11 = (A_11 + A_12/Z_load - Z_source*A_21 - Z_source*A_22/Z_load) / denom
        s21 = 2*np.sqrt(np.real(Z_source)*np.real(Z_load)) / denom
        
        s11_array.append(s11)
        s21_array.append(s21)
    
    return np.array(s11_array), np.array(s21_array)

f = np.linspace(2.0e9, 2.9e9, 1000)  # Frequency range around 2.4 GHz

# Espressif dev board values
# C1 = 3.3e-12  # 3.3 pF
# L1 = 2.2e-9   # 2.2 nH
# C2 = 3.3e-12  # 3.3 pF

# Smith chart calcualted values
C1 = 2.1e-12    # 2.1 pF
L1 = 2.86e-9     # 2.6 nH
C2 = 1.56e-12   #1.56 pF


# ESP32c3 source impedance (35 +- j10 ohms) (esp datasheet)
Z_source1 = 35 + 10j
Z_source2 = 35 - 10j
Z_source3 = 35 - 0j

# Calculate S-parameters for source impedance range
s11_1, s21_1 = calculate_s_parameters(f, C1, L1, C2, Z_source1)
s11_2, s21_2 = calculate_s_parameters(f, C1, L1, C2, Z_source2)
s11_3, s21_3 = calculate_s_parameters(f, C1, L1, C2, Z_source3)

# Plot results
plt.figure(figsize=(10, 6))
plt.plot(f/1e9, 20*np.log10(np.abs(s11_1)), label='35 + j10')
plt.plot(f/1e9, 20*np.log10(np.abs(s11_2)), label='35 - j10')
plt.plot(f/1e9, 20*np.log10(np.abs(s11_3)), label='35 - j0')
plt.legend()
plt.grid(True)
plt.xlabel('Frequency (GHz)')
plt.ylabel('S11 (dB)')
plt.title('Return Loss vs Frequency')

plt.axvline(x=2.401, color='r', linestyle='--', alpha=0.3)
plt.axvline(x=2.484, color='r', linestyle='--', alpha=0.3)
plt.axhline(y=-10, color='g', linestyle='--', alpha=0.3)

plt.show()

