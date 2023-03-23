#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    unsigned char _acVersion[28]; //Come back to this

    unsigned char tyreCompound[64]; //Come back to this

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

typedef struct payload_t {
    uint32_t id;
    uint32_t counter;
    float temp;
    uint32_t counter2;
} payload;

typedef struct trigger_t {
    uint32_t trig_code;
} trigger;

#pragma pack()

//Struct from acti
/*86 * float 4
23 * long 4
92 * char 1*/


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
    bzero((char *) &server, sizeof(server));
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
    // setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));
    bzero((char *) &server, sizeof(server));
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

void closeSocket(int sock)
{
    close(sock);
    return;
}

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

int main()
{
    int PORT = 27150;
    int PORT1 = 27151;
    int BUFFSIZE = 1024;
    char buff[BUFFSIZE];
    int ssock, ssock2, csock, csock2;
    int nread;
    
    struct sockaddr_in client;
    int clilen = sizeof(client);
    
    struct sockaddr_in client2, cliaddr;
    int clilen2 = sizeof(client2);
    
    int trigger_flag = 0;

    uint32_t trig_ack = 5550;

    ssock = createSocketTCP(PORT);
    printf("Server listening on port %d\n", PORT);


    printf("Before accept\n");
    csock = accept(ssock, (struct sockaddr *)&client, &clilen);
    if (csock < 0)
    {
        printf("Error: accept() failed\n");
        // continue;
    }

    printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
    bzero(buff, BUFFSIZE);
    while ((nread=read(csock, buff, BUFFSIZE)) > 0)
    {
        // printf("\nReceived %d bytes\n", nread);
        trigger *t = (trigger*) buff;
        trigger *ack = (trigger*) &trig_ack;
        // printf("Received contents: id=%d\n",
        //         t->trig_code);

        if (t->trig_code == 5500)
        {
            printf("Connect\n");
            trigger_flag = 1;
            sendMsg(csock, ack, sizeof(trigger));
            break;
        }

        // } else if (p->trig_code == 5501) {
        //     printf("Disconnect");
        //     trigger_flag = 0;
        //     sendMsg(csock, 5551, sizeof(int));
        // } else {
        //     print("Invalid code")
        // }
    }
    closeSocket(csock);


    closeSocket(ssock);
    memset(&client, 0, sizeof(client));
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    // sleep(5);

    ssock2 = createSocketUDP(PORT1);
    printf("Server listening on port %d\n", PORT1);

    int len, n; 
    
    len = sizeof(cliaddr);  //len is value/resuslt 
    // struct acti_payload 
    if (trigger_flag == 1) {
        while (1)
        {
            printf("\n");
            n = recvfrom(ssock2, (char *)buff, BUFFSIZE,  
                       MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                       &len);
            acti_payload *acti_pay = (acti_payload*) buff;
            printf("n = %d\n", n);
            printf("acti_payload = %ld\n", sizeof(acti_payload));
            // printf("buff = %ld\n", sizeof(buff));
            

            // printf("_acVersion = %s\n", acti_pay->_acVersion);
            // printf("tyreCompound = %s\n", acti_pay->tyreCompound);
            // printf("windSpeed = %f\n", acti_pay->windSpeed);
            // printf("windDirection = %f\n", acti_pay->windDirection);
            char* ac_version = acti_pay->_acVersion;
            char* tyre_compound = acti_pay->tyreCompound;
            // for(int i = 0; i < 28; i++)
            // {
            //     printf("ac_version[%d] = %02X\n", i, (unsigned char)ac_version[i]);
            // }

            // for(int i = 0; i < 64; i++)
            // {
            //     printf("tyre_compound[%d] = %02X\n", i, (unsigned char)tyre_compound[i]);
            // }
            
            // printf("ballast = %f\n", acti_pay->ballast);
            printf("gear = %d\n", acti_pay->gear);
            // printf("performanceMeter = %f\n", acti_pay->performanceMeter);
            // printf("status = %d\n", acti_pay->status);
            // printf("session = %d\n", acti_pay->session);
            // printf("ersRecoveryLevel = %d\n", acti_pay->ersRecoveryLevel);
            // printf("ersPowerLevel = %d\n", acti_pay->ersPowerLevel);
            printf("Gas = %f\n", acti_pay->Gas);
            printf("Brake = %f\n", acti_pay->Brake);
            printf("Steering Angle = %f\n", acti_pay->SpeedKMH);
            printf("TC = %f\n", acti_pay->tc);
            printf("ABS = %f\n", acti_pay->abs);
            printf("RPM = %f\n", acti_pay->RPM);
            printf("SpeedKMH = %f\n", acti_pay->SpeedKMH);
            // printf("ersIsCharging = %d\n", acti_pay->ersIsCharging);
            // printf("ersIsCharging = %d\n", acti_pay->ersIsCharging);
            // printf("kersCharge = %f\n", acti_pay->kersCharge);
            // printf("kersInput = %f\n", acti_pay->kersInput);
            // printf("drs = %f\n", acti_pay->drs);
            // printf("windSpeed = %f\n", acti_pay->windSpeed);
            // printf("SuspensionTravel 1 = %f\n", acti_pay->suspensionTravel0);
            // printf("SuspensionTravel 2 = %f\n", acti_pay->suspensionTravel1);
            // printf("SuspensionTravel 3 = %f\n", acti_pay->suspensionTravel2);
            // printf("SuspensionTravel 4 = %f\n", acti_pay->suspensionTravel3);
            // printf("driveTrainSpeed = %f\n", acti_pay->driveTrainSpeed);
            // printf("Local Velocity 1 = %f\n", acti_pay->localVelocity0);
            // printf("Local Velocity 2 = %f\n", acti_pay->localVelocity1);
            // printf("Local Velocity 3 = %f\n", acti_pay->localVelocity2);
            // printf("localAngularVelocity 0 = %f\n", acti_pay->localAngularVelocity0);
            // printf("localAngularVelocity 1 = %f\n", acti_pay->localAngularVelocity1);
            // printf("localAngularVelocity 2 = %f\n", acti_pay->localAngularVelocity2);
            // printf("dynamicPressure 0 = %f\n", acti_pay->dynamicPressure0);
            // printf("dynamicPressure 1 = %f\n", acti_pay->dynamicPressure1);
            // printf("dynamicPressure 2 = %f\n", acti_pay->dynamicPressure2);
            // printf("dynamicPressure 3 = %f\n", acti_pay->dynamicPressure3);
            // printf("maxRpm = %f\n", acti_pay->maxRpm);
            // printf("Laptime = %d\n", acti_pay->lapTime);
            // printf("maxRpm = %f\n", acti_pay->maxRpm);
            // char lapTime[7];
            // strncpy(lapTime, buff, 7);
            // printf("Laptime")
            // printf("Char: %s\n", buff);
            // printf("\n");
            // printf("\n");
            // printf("\n");
            // printf("\n");
            // sleep(5);
        }

        closeSocket(csock2);
        closeSocket(ssock2);

    }
    
    closeSocket(ssock2);
    printf("bye");
    return 0;
}