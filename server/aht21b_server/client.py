import requests
import json

url = "http://192.168.1.21"
response = requests.get(url)
values = json.loads(response.text)
print(f"room: {values['label']}\ntemperature: {values['temp']}C\nhumidity: {values['hum']}%")
