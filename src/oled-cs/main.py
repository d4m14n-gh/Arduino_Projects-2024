import json
from flask import Flask, request
import serial

# Otwórz połączenie z portem szeregowym (sprawdź, jaki port jest używany przez twoje Arduino)
ser = serial.Serial('COM5', 9600)  # Upewnij się, że port i prędkość są zgodne z ustawieniami Arduino

app = Flask(__name__)

@app.route('/', methods=['POST'])
def gsi():
    data = request.json
    if data and 'player' in data:
        player = data['player']
        health = player['state']['health'] if 'state' in player else 'No Data'
        active_weapon = None
        ammo_clip = 0
        ammo_clip_max = 1
        paintkit = None

        if 'weapons' in player:
            for weapon, details in player['weapons'].items():
                if details.get('state') == 'active':
                    active_weapon = details.get('name')
                    ammo_clip = details.get('ammo_clip')
                    ammo_clip_max = details.get('ammo_clip_max')
                    paintkit = details.get('paintkit')
                    print(player)
                    break

        print(f"Player Health: {health}")
        print(f"Active Weapon: {active_weapon}")
        print(f"Ammo in Clip: {ammo_clip}")

        ser.write("start\n".encode("utf-8"))
        ser.write(f"Health: {health}\n".encode("utf-8"))
        ser.write(f"Weapon: {str(active_weapon).split('_')[-1]}\n".encode("utf-8"))
        ser.write(f"Skin: {paintkit}\n".encode("utf-8"))
        ser.write(f"Ammo: {ammo_clip}/{ammo_clip_max}\n".encode("utf-8"))
        ser.write("end\n".encode("utf-8"))
        state=round((ammo_clip/ammo_clip_max)*10)
        ser.write(f"{state}\n".encode("utf-8"))
        ser.flush()
    return '', 200


if __name__ == '__main__':
    app.run(port=3000)
