import tkinter as tk
import serial
import math

# Ustawienia portu szeregowego
serial_port = 'COM6'  # Zmień na odpowiedni port
baud_rate = 9600
ser = serial.Serial(serial_port, baud_rate)
ser.dtr = False  # Wyłącza DTR

# Tworzenie okna tkinter
root = tk.Tk()
root.title("Plane visualization")
l1 = tk.Label(text="speed: 0")
l1.pack()
canvas = tk.Canvas(root, width=400, height=400)
canvas.pack()


def draw_plane(canvas, color: str="lightgray", offset=(200, 200)):
    canvas.create_oval(offset[0]-20, offset[1]-20, offset[0]+20, offset[1]+20, fill=color, outline="#1e1e1e", width=2)
    canvas.create_rectangle(offset[0]-20, offset[1], offset[0]+20, offset[1]+150, fill=color, outline="#1e1e1e", width=2)
    canvas.create_rectangle(offset[0]-40, offset[1]+150, offset[0]+40, offset[1]+180, fill=color, outline="#1e1e1e", width=2)
    canvas.create_rectangle(offset[0]-80, offset[1]+50, offset[0]+80, offset[1]+100, fill=color, outline="#1e1e1e", width=2)
    #canvas.create_polygon(150, 200, 100, 150, 200, 200, fill=color, outline="#1e1e1e", width=2)
    #canvas.create_polygon(200, 200, 300, 150, 250, 200, fill=color, outline="#1e1e1e", width=2)


draw_plane(canvas)


# Funkcja do rysowania strzałki
def draw_arrow(canvas, x, y, length, angle, color="#1e1e1e"):
    angle_rad = math.radians(angle)
    x_end = x + length * math.cos(angle_rad)
    y_end = y - length * math.sin(angle_rad)
    arrow = canvas.create_line(x, y, x_end, y_end, arrow=tk.LAST, fill="#1e1e1e", width=7)
    arrow = canvas.create_line(x, y, x_end, y_end, arrow=tk.LAST, fill=color, width=5)
    return arrow

# Funkcja aktualizująca dane
def update_arrows():
    if ser.in_waiting:
        try:
            data = ser.readline().decode('utf-8').strip()
        except:
            root.after(100, update_arrows)
            return

        print(f"Dane odebrane: {data}")
        
        if data[:5].lower()!="troll":
            root.after(100, update_arrows)
            return
        # Przykład danych: "tRoll:16 tPitch:4 roll:0 pitch:0 yaw:180 a1:58 a2:122 a3:122 a4:58"
        try:
            parts = data.split(' ')
            tRoll = float(parts[0].split(':')[1])
            tPitch = float(parts[1].split(':')[1])
            roll = float(parts[2].split(':')[1])
            pitch = float(parts[3].split(':')[1])
            yaw = float(parts[4].split(':')[1])
            a1 = float(parts[5].split(':')[1])
            a2 = float(parts[6].split(':')[1])
            a3 = float(parts[7].split(':')[1])
            a4 = float(parts[8].split(':')[1])
            speed = float(parts[9].split(':')[1])
        except:
            print("Błąd w odczycie danych")
            return

        canvas.delete("all")

        # Strzałki dla tRoll i tPitch
        draw_plane(canvas, color="#6c8b63")
        l1.config(text=f"speed: {speed}")
        draw_arrow(canvas, 200, 100, 100, 90-tRoll, color="red")
        draw_arrow(canvas, 200, 100, 100, tPitch, color="blue")
        
        # Strzałki dla roll, pitch, yaw
        draw_arrow(canvas, 200, 100, 50, 90-roll, color="green")
        draw_arrow(canvas, 200, 100, 50, pitch, color="purple")
        draw_arrow(canvas, 200, 200, 50, yaw-90, color="orange")
        
        # Strzałki dla a1-a4
        draw_arrow(canvas, 120, 275, 30, 180-(a1-90), color="red")
        draw_arrow(canvas, 280, 275, 30, a2-90, color="red")
        draw_arrow(canvas, 160, 365, 30, 180-(a3-90), color="red")
        draw_arrow(canvas, 240, 365, 30, (a4-90), color="red")

    root.after(5, update_arrows)

# Uruchamianie aktualizacji co 100ms
root.after(100, update_arrows)
root.mainloop()
