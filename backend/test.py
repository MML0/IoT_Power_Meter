import requests
import random

# Replace this URL with the actual location of your PHP script
url = "http://127.0.0.1:3000/backend/data.php"

# Function to generate randomized data
def generate_data(name = "Device A"):
    return {
        "name": name,
        "voltage": round(220.5 * (1 + random.uniform(-0.1, 0.1)), 3),  # ±10% variation
        "current": round(1.25 * (1 + random.uniform(-0.02, 0.02)), 3),  # ±2% variation
        "power": round(275.6 * (1 + random.uniform(-0.05, 0.05)), 3),  # Example ±5%
        "energy": round(3.5 * (1 + random.uniform(-0.03, 0.03)), 3),  # Example ±3%
        "frequency": round(50.0 * (1 + random.uniform(-0.01, 0.01)), 3),  # ±1%
        "power_factor": round(0.95 * (1 + random.uniform(-0.005, 0.005)), 3),  # ±0.5%
    }

# Send data 10 times
for _ in range(10):
    data = generate_data("Device A")
    response = requests.post(url, json=data)
    print("Status code:", response.status_code)
    print("Response JSON:", response.json())

    data = generate_data("Device B")
    response = requests.post(url, json=data)
    print("Status code:", response.status_code)
    print("Response JSON:", response.json())

    data = generate_data("Device C")
    response = requests.post(url, json=data)
    print("Status code:", response.status_code)
    print("Response JSON:", response.json())
