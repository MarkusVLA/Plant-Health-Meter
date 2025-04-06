# Fit a polynomial regression model for the moisture sensor voltage and capactitance.

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.pipeline import make_pipeline

data = pd.read_csv('moisture_sensor_output.txt', delim_whitespace=True, skiprows=1, 
                   names=['step', 'voltage', 'from', 'to'])

capacitance = np.linspace(40, 300, 15).reshape(-1, 1)
voltage = np.array(data['voltage']).reshape(-1, 1)
degree = 3  # polynomial degree 
poly_model = make_pipeline(PolynomialFeatures(degree), LinearRegression())

# Capactitance as a function of voltage.
poly_model.fit(voltage, capacitance)  
coefficients = poly_model.named_steps['linearregression'].coef_
intercept = poly_model.named_steps['linearregression'].intercept_

print(f"R^2 score: {poly_model.score(voltage, capacitance)}\n")
print(f"Coefficients: {coefficients}")
print(f"Intercept: {intercept}")

equation = f"capacitance = {intercept[0]:.6f}"
for i, coef in enumerate(coefficients[0][1:], 1):  
    equation += f" + {coef:.6f}×voltage^{i}"
print(f"\nPolynomial equation: {equation}")

plt.figure(figsize=(10, 6))

plt.scatter(voltage, capacitance, color='blue', label='Data points')

voltage_range = np.linspace(1.5, 3.5, 100).reshape(-1, 1) 
predicted_capacitance = poly_model.predict(voltage_range)

plt.plot(voltage_range, predicted_capacitance, color='red', label=f'Polynomial regression (degree={degree})')

plt.xlabel('Voltage')
plt.ylabel('Capacitance')
plt.title('Capacitance vs Voltage model')
plt.legend()
plt.grid(True)

plt.show()
