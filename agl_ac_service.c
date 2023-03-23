#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


void sendMsg(int sock, void* msg, uint32_t msgsize);
void closeSocket(int sock);
int createSocketTCP(int port);
void* read_trigger_port(void* p);
void* read_datastream_port(void* p);
void* foo(void* p);

// Global variable:
int i = 2;
int trigger_flag = 0;
int data_stream_exit_flag = 0;

#pragma pack(1)

typedef struct acti_payload_t {
    uint32_t lapTime;    
    uint32_t lastSectorTime;    
    float performanceMeter;   
    uint32_t status;   
    uint32_t session;
    uint32_t sessionTimeLeft;    
    uint32_t position;    
    uint32_t flags;   
    uint32_t penaltiesEnabled;
    float airDensity;  
    float airTemp; 
    float roadTemp;
    float surfaceGrip;  
    float windSpeed; 
    float windDirection;

    unsigned char _acVersion[28];
    unsigned char tyreCompound[64];

    float ballast;  
    uint32_t gear;
    float driveTrainSpeed;
    float turboBoost;
    float fuel;
    uint32_t engineBrake;
    float Gas;
    float Brake;
    float tc;
    float abs;
    uint32_t ersRecoveryLevel;
    uint32_t ersPowerLevel;
    float RPM;
    float SpeedKMH;
    float Steer;
    uint32_t drsAvailable;
    uint32_t lapInvalidated;
    uint32_t numberOfTyresOut;

    float suspensionTravel0;
    float suspensionTravel1;
    float suspensionTravel2;
    float suspensionTravel3;

    float rideHeight0;
    float rideHeight1;

    float caster;

    float toeInDeg0;
    float toeInDeg1;
    float toeInDeg2;
    float toeInDeg3;

    float pitch;
    float roll;

    float localAngularVelocity0;
    float localAngularVelocity1;
    float localAngularVelocity2;

    float localVelocity0;
    float localVelocity1;
    float localVelocity2;

    float currentTyresCoreTemp0;
    float currentTyresCoreTemp1;
    float currentTyresCoreTemp2;
    float currentTyresCoreTemp3;

    float dynamicPressure0;
    float dynamicPressure1;
    float dynamicPressure2;
    float dynamicPressure3;

    float tyreWear0;
    float tyreWear1;
    float tyreWear2;
    float tyreWear3;

    float brakeTemp0;
    float brakeTemp1;
    float brakeTemp2;
    float brakeTemp3;

    float tyreTempI0;
    float tyreTempI1;
    float tyreTempI2;
    float tyreTempI3;

    float tyreTempM0;
    float tyreTempM1;
    float tyreTempM2;
    float tyreTempM3;

    float tyreTempO0;
    float tyreTempO1;
    float tyreTempO2;
    float tyreTempO3;
    
    float carDamage0;
    float carDamage1;
    float carDamage2;
    float carDamage3;
    float carDamage4;

    float maxTorque;
    float maxPower;
    float maxTurboBoost;
    float maxRpm;
    float maxFuel;

    float suspensionMaxTravel0;
    float suspensionMaxTravel1;
    float suspensionMaxTravel2;
    float suspensionMaxTravel3;

    float kersMaxJ;
    float ersMaxJ;

    float aero_CD;
    float aero_CL_Front;
    float aero_CL_Rear;

    uint32_t autoShifterOn;
    uint32_t idealLineOn;
    float aidFuelRate;
    float aidTireRate;
    float aidMechanicalDamage;
    uint32_t aidAllowTyreBlankets;
    float aidStability;
    uint32_t aidAutoClutch;
    uint32_t aidAutoBlip;
} acti_payload;

typedef struct trigger_t {
    uint32_t trig_code;
} trigger;

#pragma pack()

struct timeval timeout={2,0}; //set timeout for 2 seconds

void sendMsg(int sock, void* msg, uint32_t msgsize)
{
    if (write(sock, msg, msgsize) < 0)
    {
        printf("Can't send message.\n");
        closeSocket(sock);
        exit(1);
    }
    // printf("Message sent (%d bytes).\n", msgsize);
    return;
}

void closeSocket(int sock)
{
    close(sock);
    return;
}

int createSocketTCP(int port)
{
    int sock, err;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR: Socket creation failed\n");
        exit(1);
    }
    printf("Socket created\n");
    // void *true = 1;
    // setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("ERROR: Bind failed\n");
        exit(1);
    }
    printf("Bind done\n");

    listen(sock , 3);

    return sock;
}

int createSocketUDP(int port)
{
    int sock, err;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("ERROR: Socket creation failed\n");
        exit(1);
    }
    printf("Socket created\n");
    // void *true = 1;
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("ERROR: Bind failed\n");
        exit(1);
    }
    printf("Bind done\n");

    listen(sock , 3);

    return sock;
}

void* read_trigger_port(void* p) {
  int PORT = 27150;
  int ssock, csock;
  int nread;

  int BUFFSIZE = 1024;
  char buff[BUFFSIZE];

  struct sockaddr_in client, cliaddr;
  int clilen = sizeof(client);
  
  uint32_t trig_connect_ack = 5550;
  trigger *conn_ack = (trigger*) &trig_connect_ack;

  ssock = createSocketTCP(PORT);
  printf("Server listening on port %d\n", PORT);

  int len, n; 

  len = sizeof(cliaddr);

  while(1)
  {
    printf("Before accept\n");
    csock = accept(ssock, (struct sockaddr *)&client, &clilen);
    if (csock < 0)
    {
        printf("Error: accept() failed\n");
        // continue;
    }
  
    if((nread=read(csock, buff, BUFFSIZE)) > 0)
    {
      trigger *t = (trigger*) buff;

      if (t->trig_code == 5500)
      {
          printf("Connect\n\n");
          trigger_flag = 1;
          sendMsg(csock, conn_ack, sizeof(trigger));
      } else if (t->trig_code == 5501) {
          printf("Disconnect\n\n");
          trigger_flag = 0;
          data_stream_exit_flag = 1;
          sendMsg(csock, conn_ack, sizeof(trigger));
          break;
      }
    }
    closeSocket(csock);
  
  }
  
  closeSocket(ssock);

  memset(&client, 0, sizeof(client));
  // Return reference to global variable:
  pthread_exit(&i);
}

void* read_datastream_port(void* p) {
  int PORT1 = 27151;
  int BUFFSIZE = 1024;
  char buff[BUFFSIZE];
  int ssock2, csock2;

  struct sockaddr_in client2, cliaddr2;
  int clilen2 = sizeof(client2);
  // Print value received as argument:

  ssock2 = createSocketUDP(PORT1);
  printf("Server listening on port %d\n", PORT1);

  int len, n; 
  len = sizeof(cliaddr2);  //len is value/resuslt

  while (1)
  {
    if (trigger_flag == 1)
    {
      printf("Before recvfrom\n\n");
      n = recvfrom(ssock2, (char *)buff, BUFFSIZE,  
                 MSG_WAITALL, ( struct sockaddr *) &cliaddr2, 
                 &len);
      acti_payload *acti_pay = (acti_payload*) buff;

      printf("gear = %d\n", acti_pay->gear);
      printf("Gas = %f\n", acti_pay->Gas);
      printf("Brake = %f\n", acti_pay->Brake);
      printf("Steering Angle = %f\n", acti_pay->SpeedKMH);
      printf("TC = %f\n", acti_pay->tc);
      printf("ABS = %f\n", acti_pay->abs);
      printf("RPM = %f\n", acti_pay->RPM);
      printf("SpeedKMH = %f\n", acti_pay->SpeedKMH);
      printf("\n");
      printf("\n");
    }
  
    printf("data_stream_exit_flag = %d\n", data_stream_exit_flag);

    if(data_stream_exit_flag == 1)
    {
      break;
    }

  }

  printf("AFTER WHILE LOOP\n");
  // Return reference to global variable:
  closeSocket(csock2);
  closeSocket(ssock2);

  pthread_exit(&i);
}

void* foo(void* p) {
  // Print value received as argument:
  printf("Value recevied as argument in starting routine: ");
  printf("%i\n", * (int*)p);

  // Return reference to global variable:
  pthread_exit(&i);
}

int main(void){
  // Declare variable for thread's ID:
  pthread_t id;
  pthread_t trigger_listener;
  pthread_t data_stream_listener;

  int j = 1;
  // pthread_create(&id, NULL, foo, &j);
  pthread_create(&trigger_listener, NULL, read_trigger_port, &j);
  // pthread_create(&data_stream_listener, NULL, read_datastream_port, &j);
    
  int* ptr;

  // Wait for foo() and retrieve value in ptr;
  // pthread_join(id, (void**)&ptr);
  pthread_join(trigger_listener, NULL);
  // pthread_join(data_stream_listener, NULL);
  // printf("Value recevied by parent from child: ");
  // printf("%i\n", *ptr);
  puts("Terminated.");
  return EXIT_SUCCESS;
}