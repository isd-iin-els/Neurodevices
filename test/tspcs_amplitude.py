
import paho.mqtt.client as mqtt
import json, time

# Configuração básica
BROKER = "localhost"   # ou IP do broker
PORT = 1883
TOPIC = "cmd2dev4556"

# Cria cliente MQTT
client = mqtt.Client()

# Conecta
client.connect(BROKER, PORT, 60)
client.loop_start()  # Inicia loop para processar mensagens (opcional, dependendo do uso)
# Payload (como dict → melhor prática)
payload = {
    "op": 2,
    "m":  "10,0,10,0",
    "t":  200,
    "p":  20000 # Hz
}



# Publica
client.publish(TOPIC, json.dumps(payload))

# Encerra
client.loop_forever()