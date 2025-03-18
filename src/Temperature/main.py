import requests
import matplotlib
import matplotlib.pyplot as plt
from datetime import datetime
import matplotlib.dates as mdates
matplotlib.use("TkAgg")

import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

plt.style.use('Solarize_Light2')

url = 'http://192.168.1.19:5000/api/TemperatureReadInfoes'
url = 'http://raspberrypi:5000/api/TemperatureReadInfoes'

print("starting...")

response = requests.get(url)
data = response.json()

#print(data)

dates = []
temperatures = []
humidities = []

for i, entry in zip(range(len(data)), data):
    read_date = datetime.fromisoformat(entry['readDate'])
    temperature = entry['temperature']
    if temperature > 5 and read_date > datetime(2024, 8, 6, 23, 5) and i % 10 == 0:
        dates.append(read_date)
        temperatures.append(temperature)
        humidities.append(entry['humidity'])

# Rysowanie wykresu
fig, ax1 = plt.subplots(figsize=(15, 9))

# Wykres temperatury
ax1.plot(dates, temperatures, 'b-', marker='.', label='Temperatura (°C)')
ax1.set_xlabel('Data i Czas')
ax1.set_ylabel('Temperatura (°C)', color='b')
ax1.tick_params(axis='y', labelcolor='b')
ax1.xaxis.set_major_locator(mdates.HourLocator(interval=48))
ax1.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
fig.autofmt_xdate()

# Tworzenie drugiej osi Y
ax2 = ax1.twinx()
ax2.plot(dates, humidities, 'g-', marker='.', label='Wilgotność (%)')
ax2.set_ylabel('Wilgotność (%)', color='g')
ax2.tick_params(axis='y', labelcolor='g')

# Dodanie legend
fig.tight_layout()
fig.legend(loc='upper left', bbox_to_anchor=(0.1, 0.9))

#plt.title('Temperatura i Wilgotność w Czasie')
plt.grid(True)

# Wyświetlenie wykresu
plt.show()


root = tk.Tk()
root.title("Wykres w Tkinter")

canvas = FigureCanvasTkAgg(fig, master=root)
canvas.draw()
canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)


quit_button = tk.Button(master=root, text="Zamknij", command=root.quit)
quit_button.pack(side=tk.BOTTOM)


tk.mainloop()