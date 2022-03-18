import paho.mqtt.client as mqtt


class Mqtt:
    def __init__(self, host: str, username: str, password: str, port=1883) -> None:
        self.host = host
        self.username = username
        self.password = password
        self.port = port
        self.ctopic = 'railway/command'
        self.stopic = 'railway/status'
        self.client = None
        self.connected = False

    def connect_mqtt(self):
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected to MQTT Broker!")
                self.connected = True
            else:
                self.connected = False
                print("Failed to connect, return code %d\n", rc)

        self.client = mqtt.Client('raspberry')
        self.client.username_pw_set(self.username, self.password)
        self.client.on_connect = on_connect
        self.client.connect(self.host, self.port)

    def publish(self, msg: str):
        self.client.publish(self.ctopic, msg)

    def subscribe(self):
        def on_message(client, userdata, msg):
            msg = msg.payload.decode()
            match msg:
                case 'on' | 'off': self.publish(msg)           

        self.client.subscribe(self.stopic)
        self.client.on_message = on_message

    def run(self):
        while not self.connected:
            self.client = self.connect_mqtt()
        self.client.loop_start()
