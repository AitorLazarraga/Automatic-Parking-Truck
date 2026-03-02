from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI
import time

max_data = 30
distancias = {
    'ToF': [],   # Trasero - muelle de carga
    '0': [], '1': [], '2': [], '3': [],  # Lateral derecho remolque
    '4': [], '5': [], '6': [], '7': [],  # Lateral izquierdo remolque
    '8': [], '9': []  # Laterales tractora
}

def on_distance(data: str):
  sens,medida = data.split(':')
  
  if sens == 'M':
    distancias["ToF"].append(medida)
    if len(distancias["ToF"]) > max_data:
      distancias["ToF"].pop(0)
      
   # print(f"Medida del sensor ToF: {medida}")
    
  else:
    sens = str(sens)
    distancias[sens].append(medida)
    if len(distancias[sens]) > max_data:
      distancias[sens].pop(0)
      
    #print(f"Lectura del sensor {sens} es {medida} cm")

def most_data():
    """Función que retorna los últimos datos de los sensores"""
    result = {}
    
    for sens in distancias:
        # Verificar si hay datos disponibles
        if len(distancias[sens]) > 0:
            result[sens] = {
                "ultimo": distancias[sens][-1],
                "total_medidas": len(distancias[sens])
            }
            #print(f"Último dato entregado por el sensor {sens} = {distancias[sens][-1]}")
        else:
            result[sens] = {
                "ultimo": "Sin datos",
                "total_medidas": 0
            }
    
    # Retornar los datos en formato JSON
    return result
    
def status(data: str):
  print(f"STATUS :: {data}")
    
def get_all_sensor_data():
    """Función que retorna TODOS los datos históricos de los sensores"""
    return {
        "sensores": distancias,
        "timestamp": time.time()
    }

def motor_control(data: dict):

    cmd = data.get("cmd", "")
    print(f"Comando motor recibido: {cmd}")

    Bridge.call("controlMotores", cmd)

    return {"status": "ok", "sent": cmd}
    
    


ui = WebUI()
ui.expose_api("GET", "/data", most_data)
ui.expose_api("GET", "/all", get_all_sensor_data)
ui.expose_api("POST", "/motor", motor_control)


Bridge.provide("distance", on_distance)
Bridge.provide("status", status)

ui.start()
App.run()
