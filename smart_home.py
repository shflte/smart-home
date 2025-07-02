from flask import Flask, jsonify
import paho.mqtt.client as mqtt

mqtt_broker = "localhost"
mqtt_port = 1883

app = Flask(__name__)

gate_slide_topic = "gate/slide"
balcony_pump_topic = "pump/balcony"
balcony_status_topic = "pump/balcony/status"

latest_status = {"status": "unknown"}

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    client.subscribe(balcony_status_topic)


def on_message(client, userdata, msg):
    global latest_status
    if msg.topic == balcony_status_topic:
        latest_status = {"status": msg.payload.decode()}
        print(f"Received pump status: {latest_status}")


mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_client.connect(mqtt_broker, mqtt_port, 60)
mqtt_client.loop_start()


def publish_message(topic, message):
    mqtt_client.publish(topic, message)
    print(f"Published message: {message} to topic: {topic}")


@app.route("/api/gate/slide/open", methods=["GET"])
def open_gate():
    publish_message(gate_slide_topic, "open")
    return "OK", 200


@app.route("/api/gate/slide/close", methods=["GET"])
def close_gate():
    publish_message(gate_slide_topic, "close")
    return "OK", 200


@app.route("/api/gate/slide/stop", methods=["GET"])
def stop_gate():
    publish_message(gate_slide_topic, "stop")
    return "OK", 200


@app.route("/api/pump/balcony/on", methods=["GET"])
def pump_on():
    publish_message(balcony_pump_topic, "on")
    return "OK", 200


@app.route("/api/pump/balcony/off", methods=["GET"])
def pump_off():
    publish_message(balcony_pump_topic, "off")
    return "OK", 200


@app.route("/api/pump/balcony/status", methods=["GET"])
def pump_status():
    return "1" if latest_status["status"] == "on" else "0", 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=3738)
