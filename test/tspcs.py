import json
import time
import paho.mqtt.client as mqtt

BROKER = "10.254.191.6"
PORT = 1883
TOPIC = "cmd2dev4240"
BROKER = "127.0.0.1"

payload = {
    "op": 2,
    "TspcsPulsewidth": 100                                      ,   # float real
    "Tspcsfrequency": 30, 
    "pulse_time": 50, 
    "m":         "2,0,10,0",
    "t":           200,
    "p":           20000 # Hz
}

def on_connect(client, userdata, flags, rc, properties=None):
    # print("Conectado com sucesso ao Broker!")
    if rc == 0:
        print("Conectado com sucesso ao Broker!")
        # Inscrever-se dentro do on_connect garante a reinscrição automática caso a conexão caia
        client.subscribe(TOPIC)
        print(f"Inscrito no tópico: {TOPIC}")
        time.sleep(0.5)
        client.publish(TOPIC, json.dumps(payload), qos=0)
       
    else:
        print(f"Falha na conexão. Código de erro: {rc}")

# Callback disparado quando uma mensagem chega
def on_message(client, userdata, msg):
    print("\n--- Nova Mensagem Recebida ---")
    print(f"Tópico:  {msg.topic}")
    print(f"Payload: {msg.payload.decode('utf-8')}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

client.on_connect = on_connect
client.on_message = on_message

# Paho-MQTT v2.x exige especificação da versão da API

client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, 60)
# client.loop_start()
client.loop_forever()
# time.sleep(0.5) # Aguarda a estabilização da conexão com o broker

# # Dispara o publish e força o aguardo da entrega
# info = client.publish(TOPIC, json.dumps(payload), qos=1)
# info.wait_for_publish() 

# print("Mensagem enviada com sucesso!")

# client.loop_stop()
# client.disconnect()