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

    uint32_t tempNum;
    
    uint32_t position;
    
    uint32_t flags;
    
    uint32_t penaltiesEnabled;

    float airDensity;
    
    float airTemp;
    
    float roadTemp;
    
    float surfaceGrip;
    
    float windSpeed;
    
    float windDirection;

    char _acVersion[27]; //Come back to this

    char tyreCompound[31]; //Come back to this

    float ballast;
    
    uint32_t gear;

    float driveTrainSpeed;
    
    float turboBoost;
    
    float fuel;

    uint32_t engineBrake;

    float brakeBias;
    
    float kersCharge;
    
    float kersInput;
    
    float drs;
    
    uint32_t ersRecoveryLevel;
    
    uint32_t ersPowerLevel;
    
    uint32_t ersHeatCharging;
    
    uint32_t ersIsCharging;

    float kersCurrentKJ;

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


int createSocket(int port)
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
    
    struct sockaddr_in client2;
    int clilen2 = sizeof(client2);
    
    int trigger_flag = 0;

    uint32_t trig_ack = 5550;

    ssock = createSocket(PORT);
    printf("Server listening on port %d\n", PORT);
    
    ssock2 = createSocket(PORT1);
    printf("Server listening on port %d\n", PORT1);

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
    memset(&client, 0, sizeof(&client));
    sleep(5);
    
    if (trigger_flag == 1) {
        while (1)
        {
            printf("Before accept2\n");
            csock2 = accept(ssock2, (struct sockaddr *)&client, &clilen2);
            if (csock2 < 0)
            {
                printf("Error: accept() failed\n");
                continue;
            }

            printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
            bzero(buff, BUFFSIZE);
            while ((nread=read(csock2, buff, BUFFSIZE)) > 0)
            {
                printf("\nReceived %d bytes\n", nread);
                acti_payload *p = (acti_payload*) buff;
                printf("Received contents: driveTrainSpeed=%f\n",
                        p->driveTrainSpeed);

                printf("Sending it back.. ");
                // sendMsg(csock2, p, sizeof(acti_payload));
            }
            printf("Closing connection to client\n");
            printf("----------------------------\n");
            closeSocket(csock2);
        }

        closeSocket(ssock2);

    }
    
    closeSocket(ssock2);
    printf("bye");
    return 0;
}