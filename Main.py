from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI
import time
import json

dictIMU = {
    "Yaw": 0,
    "Gyro_Z": 0,
    "CSystem": 0,
    "CGyro": 0,
    "CAccel": 0,
    "CMag": 0,
    "timestamp": 0
}

max_data = 30
distancias = {
    'ToF': [],
    '0': [], '1': [], '2': [], '3': [],
    '4': [], '5': [], '6': [], '7': [],
    '8': [], '9': []
}

# ── WebSocket handlers ──────────────────────────────────────────────
def on_ws_connect(sid):
    print(f"Cliente conectado: {sid}")

def on_ws_disconnect(sid):
    print(f"Cliente desconectado: {sid}")

# ── Sensor callbacks ────────────────────────────────────────────────
def get_imu():
    """
    Obtiene el diccionario de los datos del sensor IMU para mostrar en el webserver
    """
    return dictIMU

def on_distance(data: str):
    """
    La función es llamada por cada sensor para obtener sus datos en python para poder usarlos
    """
    try:
        sens, medida = data.split(':')
    except:
        return
    key = "ToF" if sens == 'M' else str(sens)
    if key in distancias:
        distancias[key].append(medida)
        if len(distancias[key]) > max_data:
            distancias[key].pop(0)

    #Se envian los datos de los sensores por websocket
    ui.send_message("sensors", most_data())

def most_data():
    result = {}
    for sens in distancias:
        if distancias[sens]:
            result[sens] = {
                "ultimo": distancias[sens][-1],
                "total_medidas": len(distancias[sens])
            }
        else:
            result[sens] = {"ultimo": "Sin datos", "total_medidas": 0}
    return result

def status(data: str):
    print(f"STATUS :: {data}")

def get_all_sensor_data():
    return {"sensores": distancias, "timestamp": time.time()}

def motor_control(data: dict):
    """
    Funcion puente entre webserver JS y arduino para control por UART
    """
    cmd = data.get("cmd", "")
    print(f"Comando motor recibido: {cmd}")
    Bridge.call("controlMotores", cmd)
    return {"status": "ok", "sent": cmd}

def parseIMU(line):
    """
    Función que obtiene las lineas leidas por serial de arduino y las parsea en el diccionario del imu para control
    """
    global dictIMU

    line = line.strip()
    if not line.startswith("IMU"):
        return None

    parts = line.split(",")
    if len(parts) != 7:
        return

    dictIMU = {
        'Yaw':     float(parts[1]),
        'Gyro_Z':  float(parts[2]),
        'CSystem': int(parts[3]),
        'CGyro':   int(parts[4]),
        'CAccel':  int(parts[5]),
        'CMag':    int(parts[6]),
    }
    
    ui.send_message("imu", get_imu())

# ── WebUI setup ─────────────────────────────────────────────────────
ui = WebUI()
ui.expose_api("GET",  "/data",  most_data)
ui.expose_api("GET",  "/all",   get_all_sensor_data)
ui.expose_api("POST", "/motor", motor_control)
ui.expose_api("GET",  "/imu",   get_imu)

ui.on_connect(on_ws_connect)
ui.on_disconnect(on_ws_disconnect)

Bridge.provide("distance", on_distance)
Bridge.provide("status",   status)
Bridge.provide("parse",    parseIMU)

ui.start()
App.run()
