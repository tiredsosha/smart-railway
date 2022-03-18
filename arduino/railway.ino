#include <Stepper.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define stepsQty 400
#define motorInterfaceType 1
#define direction 2
#define steps 3
#define ENDSTART 4
#define ENDSTOP 5
#define USERNAME "admin"
#define PASSWORD "admin"
#define mqttServer "192.168.1.1"

// Set your MAC address and IP address here
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 160);

// Ethernet and MQTT related objects
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver
Stepper stepper(stepsQty, direction, steps);

// Motor related functions
void goBack()
{
    stepper.step(-500);
}
void goForward()
{
    stepper.step(500);
}
void goBase()
{
    while (!digitalRead(ENDSTART))
    {
        goBack()
    }
    delay(10);
    for (int i = 0; i <= 3; i++)
    {
        goForward()
    }
}

// Mqtt related funcs
bool newJob false;

void callback(char *topic, byte *message, unsigned int length)
{
    // If a message is received on the topic railway/command, change global variable.
    if (String(topic) == "railway/command")
    {
        String command;

        for (int i = 0; i < length; i++)
        {
            command += (char)message[i];
        }

        if (command == "on")
        {
            Serial.println("new message arrived - starning scanning");
            newJob = true;
        }
        else if (command == "off")
        {
            Serial.println("new message arrived - stopping scanning");
            newJob = false;
        }
    }
}
void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("Railway"))
        {
            Serial.println("connected");
            client.subscribe("railway/command");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

void setup()
{
    // Serial
    Serial.begin(9600);
    // Ethenet connection
    Ethernet.begin(mac, ip);
    delay(3000);
    // Mqtt config
    mqttClient.setmqttServer(mqttServer, 1883);
    client.setCallback(callback);
    // Set motor:
    stepper.setSpeed(600);
    // Set endpoints
    pinMode(ENDSTART, INPUT);
    pinMode(ENDSTOP, INPUT);

    goBase();
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    if (newJob)
    {
        goBase();
        client.publish("railway/status", "scanning");
        while (newJob)
        {
            if (!digitalRead(ENDSTOP))
            {
                goForward();
            }
            delay(10);
            else
            {
                newJob = false;
                client.publish("railway/status", "having night out with girlzzz");
            }
        }
        goBase();
    }
    delay(500);
}
