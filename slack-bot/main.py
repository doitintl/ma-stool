import json
import logging
import threading
from datastore import  State
import paho.mqtt.client as mqtt
from flask import Flask, request
from google.cloud import tasks_v2beta3
from slackclient import SlackClient


app = Flask(__name__)

app.logger.setLevel(logging.INFO)

with open('config.json', 'r') as f:
    data = f.read()
config = json.loads(data)

lock = threading.Lock()


def on_mqtt_message(mqttc, obj, msg):
    logging.info("Get a new message Topic: " + msg.topic + " QOS: " + str(msg.qos) + " Payload: " + str(msg.payload))
    client = tasks_v2beta3.CloudTasksClient()
    parent = client.queue_path("aviv-playground", 'europe-west2', 'ma-stool')
    task = {
        'app_engine_http_request': {  # Specify the type of request.
            'http_method': 'POST',
            'relative_uri': '/on_message'
        }
    }
    st = State()
    st.put(msg.payload.decode("utf-8"))
    task['app_engine_http_request']['body'] = ""
    response = client.create_task(parent, task)
    logging.info('Created task {}'.format(response.name))
    return response


@app.route('/on_message', methods=['POST'])
def on_message():
    send_status_to_slack(config['SLACK_CHANNEL'])
    return 'ok', 200


def send_status_to_slack(channel):
    slack_client = SlackClient(config['SLACK_BOT_TOKEN'])
    st = State()
    door_status = st.get()
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
    logging.info("WarmUp")
    return '', 200, {}


@app.route('/slack/command', methods=['POST'])
def ma_stool():
    if request.form['channel_name'] == 'directmessage':
        channel = request.form['user_id']
    else:
        channel = request.form['channel_id']
    send_status_to_slack(channel)
    return '', 200, {}

@app.route('/_ah/start')
def mqtt_worker():
    logging.info("Staring mqtt_worker")
    while True:
        logging.info(" Starting MQTT Loop")
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
        logging.warning("Out of mqttc.loop_forever()")


if __name__ == "__main__":
    logging.info('Staring service')
    app.run(debug=True)
