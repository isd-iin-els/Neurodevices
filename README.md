# fes4channels
 
O resumo do script é sobre a criação de um serviço com Fes4Channels, que envolve a implementação de um dispositivo e a comunicação com um microcontrolador. O vídeo aborda a criação de um serviço básico, a comunicação com Python via MQTT e a adaptação do projeto para o Unit. Além disso, o objetivo é publicar um artigo em um congresso de engenharia biomédica até o final do mês. A equipe discutirá a implementação e as próximas etapas na próxima semana.

Desenvolvimento de Aplicações com Microcontroladores e MQTT Discussão sobre infraestrutura de projetos de saúde e neuroengenharia. Utilização de Python, C++ e Arduino para enviar e receber dados via MQTT.

Configuração e utilização de bibliotecas em um projeto de engenharia Instalação de dependências necessárias para o projeto. Utilização de bibliotecas para comunicação serial e processamento de sinais. Problemas ao vincular dependências e realizar o Git Clone.

Configuração de Plataforma para Programação de Dispositivos Configuração de sistemas embarcados exclusivamente para trabalhar com sistemas embarcados. Plataforma de programação para dispositivos, gerenciador de dispositivos e opções de velocidade de processador e transmissão de dados. Possibilidade de programar para diferentes placas de circuito e escolher entre bibliotecas originais ou do fabricante.

Criação de um novo serviço e comunicação entre dispositivos Criação de um novo serviço com base em template Dispositivos se comunicam via portas do microcontrolador Aplicação Python faz publish e subscribe para interação com sensor

Configuração de Serviços e Bibliotecas para Microcontroladores Configuração de serviços e funções de callback para gerenciar operações em microcontroladores. Template de serviço para inicialização e execução de funções em microcontroladores seguindo a filosofia do Arduino. Biblioteca matemática e de eletroestimulação para trabalhar com matrizes e vetores em microcontroladores. Configuração de serviços e funções para lidar com sensores e atuadores em microcontroladores.

Configuração de Leitura de Sensores e Comunicação com Microcontrolador Configuração do dispositivo para leitura de tensão e armazenamento em 12 bits. Necessidade de comunicação específica para sensores e definição de operações em JSON. Preparação para simulação e comunicação com microcontrolador através de comandos específicos.

Configurando e Inicializando o ADC em um Microcontrolador Debugar para identificar problemas na função ADC. Incluir arquivo ADC para inicialização correta.

Configuração de Dispositivo e Problemas de Conexão Problemas com o controlador e conexão Wi-Fi. Processo de reinicialização e configuração do dispositivo. Necessidade de atualizar o contador de loop no arquivo template.

Configuração e funcionamento de serviços em Python para envio de dados via MQTT Atualização desconhecida causou problemas na gravação de dados. Possibilidade de enviar dados via MQTT com script Python. Configuração de serviços para envio de dados e funcionamento dos sensores.

Implementação de Serviços e Comunicação de Dados em Dispositivos Perspectiva sobre envio de dados e funções de inicialização de sensores. Processo de envio e recebimento de dados entre dispositivos e Python. Adaptação de serviços para integração com o Unit e Open Vibe.

Implementação de Serviços e Publicação de Artigo em Congresso de Engenharia Biomédica Simplificação do script Python e exclusão de arquivos desnecessários. Adição de informações essenciais ao script para visualização e publicação de artigo. Discussão sobre publicação de artigo em congresso de engenharia biomédica e planejamento de implementação.

## Procedimentos para conexão Neurodevices MQTT com ESP e aquisição de dados via _websocket_

1. Clonar repositório [Neurodevices](https://github.com/isd-iin-els/Neurodevices/tree/main)
2. Abrir projeto no VSCode
3. Conectar ESP à porta USB
4. Caso o projeto ainda não esteja gravado na memória _flash_ do ESP
   - Fazer upload do projeto Neurodevices para o ESP a partir do VSCode
   - Conectar (preferivelmente celular) à rede Wi-Fi devXXXX (XXXX é o número do dispositivo ESP. Ex.: dev3952)
   - Acessar o endereço 8.8.8.8 no _browser_ (chrome, firefox, etc...)
   - Preencher seguintes campos do formulário para configuração do ESP
     - WiFi SSID: CAMPUS
     - WiFi _Password_: IINELS_educacional
     - MQTT _Server Host Name_: endereço IP da máquina onde está instalado o servidor _broker_ MQTT (verificar IP conectado à rede Wi-Fi CAMPUS)
     - MQTT _Server port_: número da porta configurada para o _broker_ MQTT
       - Nesse caso, deve-se configurar a porta 1883, padrão do MQTT
     - _Submit_
   - Estando a máquina do _broker_ conectada à rede CAMPUS, abrir interface `directStimulation.html` no _browser_ e preencher as seguintes informações para coleta de dados com a palmilha:
     - _Server address_: endereço IP da máquina onde está instalado o servidor _broker_ MQTT (verificar IP conectado à rede Wi-Fi CAMPUS)
     - _Port_: número da porta configurada para o _broker_ MQTT
       - Caso o arquivo de configuração do _broker_ MQTT esteja configurado para conexão _websocket_ na porta 1887, configurar essa porta.
     - Tópico da palmilha: número do dispositivo ESP (Ex.: 3952)
     - Tempo de experimento: em segundos
   - Connect to MQTT
   - Começar experimento
