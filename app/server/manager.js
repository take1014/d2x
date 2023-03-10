const mqtt = require('mqtt');
const config = require('../app_conf.json');

// mqtt modules
let mqttSub = mqtt.connect(config['SERVER']['DB_MANAGER']['SUB_IP']);
mqttSub.on('connect', function() {
    console.log("success server mqtt connetion.")
    mqttSub.subscribe(config['SERVER']['DB_MANAGER']['SUB_KEY']);
});

mqttSub.on('message', function(topic, message) {
    manageDB(message.toString());
});

function manageDB(data)
{
    console.log(data);
}