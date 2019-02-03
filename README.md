# Ma-Stool
Bathroom occupancy detector [Blog Post](https://medium.com/@avivl/harvesting-the-power-of-the-cloud-and-arduino-for-bathroom-occupancy-awareness-47bc2a873799)


## Arduino
1. Install [PlatfromIO](https://platformio.org/)
2. Create `ma-stool-config.cpp` file as explained in `ma-stool-config.h`
3. `cd server`
4. `pio run`

## Slack App
1. `cd slack-bot`
2. Create `config.json` 

```
{
  "SLACK_TOKEN": "",
  "SLACK_BOT_TOKEN": "",
  "SLACK_CHANNEL": "",
  "MQTT_SERVER": "",
  "MQTT_PORT": ,
  "MQTT_USER":"",
  "MQTT_PASSWORD": "",
  "MQTT_TOPIC": ""

}
```
2. `app deploy -q app.yaml dispatch.yaml mqtt-worker.yaml`


