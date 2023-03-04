const mqtt = require('mqtt');

// mqtt modules
let mqttSub = mqtt.connect('ws://big-ucsk.local:15675');
mqttSub.on('connect', function() {
    console.log("success server mqtt connetion.")
    mqttSub.subscribe('gps');
});


mqttSub.on('message', function(topic, message) {
    manageDB(message.toString());
});

function manageDB(data)
{
    console.log(data);
}

