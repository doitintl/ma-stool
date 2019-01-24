import json
import logging
import threading

import paho.mqtt.client as mqtt
from flask import Flask, request
from google.cloud import tasks_v2beta3
from slackclient import SlackClient

app = Flask(__name__)
with open('config.json', 'r') as f:
    data = f.read()
config = json.loads(data)

door_status = ""
lock = threading.Lock()


def on_mqtt_message(mqttc, obj, msg):
    logging.warning(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    client = tasks_v2beta3.CloudTasksClient()
    parent = client.queue_path("aviv-playground", 'europe-west2', 'ma-stool')
    task = {
        'app_engine_http_request': {  # Specify the type of request.
            'http_method': 'POST',
            'relative_uri': '/on_message'
        }
    }
    converted_payload = str(msg.payload).encode()
    task['app_engine_http_request']['body'] = converted_payload
    response = client.create_task(parent, task)
    logging.warning('Created task {}'.format(response.name))
    return response


@app.route('/on_message', methods=['POST'])
def on_message():
    payload = request.get_data(as_text=False) or '(empty payload)'
    lock.acquire()
    global door_status
    door_status = str(payload, 'utf-8')
    door_status = door_status[:-1]
    door_status = door_status[2:].lower()
    lock.release()
    send_status_toslack(config['SLACK_CHANNEL'])
    return 'ok', 200


def send_status_toslack(channel):
    slack_client = SlackClient(config['SLACK_BOT_TOKEN'])
    if door_status == '1':
        text = ' Bathroom is free :woman-running: '
    elif door_status == '0':
        text = ' Bathroom is occupied :hourglass_flowing_sand: '
    else:
        text = 'Bathroom status unknown :question: '

    slack_client.api_call(
        "chat.postMessage",
        channel=channel,
        text=":poop: " + text
    )

def on_log(mqttc, obj, level, string):
    logging.info(string)


def verify_slack_token(request_token):
    if request_token != config['SLACK_TOKEN']:
        raise ValueError('Invalid request/credentials.')


@app.route('/_ah/warmup')
def warmup():
    logging.debug("WarmUp")
    return '', 200, {}


@app.route('/slack/command', methods=['POST'])
def ma_stool():
    if request.form['channel_name'] == 'directmessage':
        channel = request.form['user_id']
    else:
        channel = request.form['channel_id']
    send_status_toslack(channel)
    return '', 200, {}

@app.route('/_ah/start')
def mqtt_worker():
    logging.warning(" Start 1")
    with open('config.json', 'r') as f:
        data = f.read()
    mqtt_config = json.loads(data)
    mqttc = mqtt.Client()
    mqttc.on_message = on_mqtt_message
    # Uncomment to enable debug messages
    #mqttc.on_log = on_log
    mqttc.username_pw_set(mqtt_config['MQTT_USER'],
                          mqtt_config['MQTT_PASSWORD'])
    mqttc.connect(mqtt_config['MQTT_SERVER'], mqtt_config['MQTT_PORT'], 60)
    mqttc.subscribe(mqtt_config['MQTT_TOPIC'], 0)
    mqttc.loop_forever()
    logging.warning("Out of the loop")


if __name__ == "__main__":
    logging.warning('Main')
    app.run(debug=True)
