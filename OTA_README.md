## Conectando o Neurodevices MQTT ao ESP e coletando dados via WebSocket

Este guia detalha como conectar o Neurodevices MQTT ao ESP e adquirir dados usando WebSocket.

**Pré-requisitos:**

* Repositório Neurodevices clonado: [https://github.com/isd-iin-els/Neurodevices/tree/main](https://github.com/isd-iin-els/Neurodevices/tree/main)
* Projeto aberto no VSCode
* ESP conectado à porta USB

**Procedimento:**

1. **Gravar o projeto no ESP:**
   - Se o projeto ainda não estiver na memória flash do ESP, faça o upload a partir do VSCode.
2. **Conectar à rede Wi-Fi do ESP:**
   - Conecte seu celular ou computador à rede Wi-Fi "devXXXX" (onde XXXX é o número do seu ESP, por exemplo, dev3952). A senha padrão é "12345678".
3. **Configurar o ESP:**
   - Abra um navegador e acesse o endereço "8.8.8.8".
   - Preencha o formulário de configuração do ESP:
     - **WiFi SSID:** CAMPUS
     - **WiFi Password:** IINELS_educacional
     - **MQTT Server Host Name:** Endereço IP da máquina com o servidor broker MQTT (conectado à rede Wi-Fi CAMPUS).
     - **MQTT Server Port:** Porta configurada para o broker MQTT (1883 por padrão).
   - Clique em "Submit".
4. **Iniciar a interface de coleta de dados:**
   - Com a máquina do broker MQTT conectada à rede CAMPUS, abra o arquivo "directStimulation.html" em um navegador.
   - Preencha as informações para coleta de dados da palmilha:
     - **Server address:** Endereço IP da máquina com o servidor broker MQTT.
     - **Port:** Porta configurada para o broker MQTT (1887 para conexão WebSocket).
     - **Tópico da palmilha:** Número do dispositivo ESP (por exemplo, 3952).
     - **Tempo de experimento:** Duração em segundos.
   - Clique em "Connect to MQTT".
   - Clique em "Começar experimento".

## Atualização OTA do Neurodevices

O Neurodevices suporta atualização de firmware OTA (over-the-air). Siga estes passos para atualizar:

1. **Ativar o modo de configuração:**
   - Pressione o botão de boot do ESP por 4 segundos.
2. **Conectar à rede Wi-Fi do ESP:**
   - Conecte seu celular ou computador à rede Wi-Fi "devXXXX". A senha padrão é "12345678".
3. **Acessar a página de upload:**
   - Abra um navegador e acesse o endereço "8.8.8.8/update".
4. **Fazer upload do firmware:**
   - Selecione o arquivo ".bin" do novo firmware.
   - Clique no botão "Update".
5. **Aguardar a atualização:**
   - Aguarde a conclusão da atualização. O ESP reiniciará automaticamente com o novo firmware.
