#ifndef servicesDef_h
#define servicesDef_h

#define RESTART_MSG     7
#define WHOAMI_MSG      9
#define BLINKME_MSG     10
#define IMUSENDINIT_MSG 1
#define OPENLOOPFESUPDATE_MSG 2
#define STOPOPENLOOPFES_MSG 8
#define CLOSEDLOOPFESCONTROL_MSG 3
#define CLOSEDLOOPFESREFERENCEUPDATE_MSG 4
#define PIDSPARAMETERSUPDATE_MSG 5
#define MP_PIDSPARAMETERSUPDATE_MSG 13
#define MP_PIDSFESREFERENCEUPDATE_MSG 14
#define MP_PIDSTWODOFLIMBFES_MSG 15
#define RENAMETOPICS_MSG 16
#define ALIVE_MSG 17
#define OPENLOOPTONFREQUPDATE_MSG 18
#define IMUACCELEROMETERCALIBRATION_MSG 19
#define IMUGIROSCOPECALIBRATION_MSG 20
#define IMUMAGNETOMETERCALIBRATION_MSG 21
#define IMUSENDSTOP__MSG 22
#define FESBIKESTART_MSG 23
#define ADCSTREAM_MSG 24
#define UPDATE_FIRMWARE_MSG 25
#define BPMSTREAM_MSG 26
#define STOPBPMSTREAM_MSG 27
#define INSOLESTREAM_MSG 28
#define STOPINSOLESTREAM_MSG 29
#define STOPADCSTREAM_MSG 30
#define IMUControllerINIT_MSG 31
#define IMUControllerSTOP__MSG 32

#define RESTART_PARAMETERS                      "{\"op\":7}"
#define WHOAMI_PARAMETERS                       "{\"op\":9}"
#define BLINKME_PARAMETERS                      "{\"op\":10}"
#define IMUSENDINIT_PARAMETERS                  "{\"op\":1,\"simulationTime\":\"float\",\"frequence\":\"float\",\"sensorType\":{\"mpu6050\":1,\"GY80\":2}}"
#define IMUControllerINIT_PARAMETERS                  "{\"op\":31,\"simulationTime\":\"float\",\"frequence\":\"float\",\"sensorType\":{\"mpu6050\":1,\"GY80\":2}}"
#define OPENLOOPFESUPDATE_PARAMETERS            "{\"op\":2,\"m\":\"Channel String Vector\",\"t\":\"Ton Uint\",\"p\":\"Period Uint\",\"f\":\"fade time microsseconds\"}"
#define STOPOPENLOOPFES_PARAMETERS              "{\"op\":8}"
#define CLOSEDLOOPFESCONTROL_PARAMETERS         "{\"op\":3,\"simulationTime\":\"float\",\"controlSampleTime\":\"float\",\"sensorSampleTime\":\"float\",\"Ton\":\"float\",\"period\":\"float\",\"kp\":\"vectorfloat\",\"ki\":\"vectorfloat\",\"kd\":\"vectorfloat\",\"minInputLimit\":\"vectorfloat\",\"maxInputLimit\":\"vectorfloat\",\"ref\":\"vectorfloat\",\"operationalP\":\"vectorfloat\"}"
#define CLOSEDLOOPFESREFERENCEUPDATE_PARAMETERS "{\"op\":4,\"r\":\"vectorfloat\"}"
#define PIDSPARAMETERSUPDATE_PARAMETERS         "{\"op\":5,\"simulationTime\":\"float\",\"controlSampleTime\":\"float\",\"sensorSampleTime\":\"float\",\"Ton\":\"float\",\"period\":\"float\",\"kp\":\"vectorfloat\",\"ki\":\"vectorfloat\",\"kd\":\"vectorfloat\",\"minInputLimit\":\"vectorfloat\",\"maxInputLimit\":\"vectorfloat\",\"ref\":\"vectorfloat\",\"operationalP\":\"vectorfloat\"}"
#define MP_PIDSPARAMETERSUPDATE_PARAMETERS 13
#define MP_PIDSFESREFERENCEUPDATE_PARAMETERS 14
#define MP_PIDSTWODOFLIMBFES_PARAMETERS 15
#define RENAMETOPICS_PARAMETERS  "{\"op\":16,\"topic\":\"string\"}"
#define ALIVE_PARAMETERS "{\"op\":17}"
#define OPENLOOPTONFREQUPDATE_PARAMETERS "{\"op\":18,\"t\":\"Ton Uint\",\"p\":\"Period Uint\"}"
#define IMUACCELEROMETERCALIBRATION_PARAMETERS "{\"op\":19,\"frequence\":\"float\",\"sensorType\":{\"mpu6050\":1,\"GY80\":2}}"
#define IMUGIROSCOPECALIBRATION_PARAMETERS "{\"op\":20,\"frequence\":\"float\",\"sensorType\":{\"mpu6050\":1,\"GY80\":2}}"
#define IMUMAGNETOMETERCALIBRATION_PARAMETERS "{\"op\":21,\"frequence\":\"float\",\"sensorType\":{\"mpu6050\":1,\"GY80\":2}}"
#define IMUSENDSTOP_PARAMETERS                  "{\"op\":22}"
#define IMUControllerStop_PARAMETERS                  "{\"op\":32}"
#define FESBIKESTART_PARAMETERS                 "{\"op\":23,\"Ton\":\"Ton Uint\",\"period\":\"Period Uint\",\"frequence\":\"float\",\"simulationTime\":\"float\",\"fesCyclingMin\":\"float\",\"fesCyclingMax\":\"float\"}"
#define ADCSTREAM_PARAMETERS  "{\"op\":24,\"frequence\":\"float\",\"timeout\":\"float\",\"devicePin\":\"uint8\"}"
#define UPDATE_FIRMWARE_PARAMETERS  "{\"op\":25,\"url\":\"hostName\",\"binLocation\":\"urlToFile\",\"md5\":\"hash\"}"
#define BPMSTREAM_PARAMETERS  "{\"op\":26,\"frequence\":\"float\",\"timeout\":\"float\",\"bpm_scale\":\"uint8\",\"bpm_threshold\":\"double\"}"
#define STOPBPMSTREAM_PARAMETERS  "{\"op\":27}"
#define INSOLESTREAM_PARAMETERS  "{\"op\":28,\"frequence\":\"float\",\"timeout\":\"float\"}"
#define STOPINSOLESTREAM_PARAMETERS "{\"op\":29}"
#define STOPADCSTREAM_PARAMETERS  "{\"op\":30}"
#endif