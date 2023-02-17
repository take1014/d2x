const mqtt = require("mqtt");
var L      = require("leaflet");
var map = L.map('map').setView({lon: 139.3578657, lat: 36.2737866}, 18);
L.tileLayer('http://192.168.3.5/hot/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
    maxZoom: 21,
    maxNativeZoom: 20
}).addTo(map);

// show a marker on the map
L.marker({lon: 139.3578657, lat: 36.2737866}).bindPopup('The center of the world').addTo(map);

// mqtt (Sub)
const mqtthost = 'localhost'
const sub = mqtt.connect('mqtt://' + mqtthost);
sub.on('connect', function(){
    console.log('mqtt ' + mqtthost + ' connected.')
    sub.subscribe('gps');
});

sub.on('error', function(err){
    console.log(err);
});

sub.on('message', function(topic, message) {
    console.log("readGPSData");
    readGPSData(message.toString());
});

function readGPSData(data)
{
    console.log(data);
    if(data == null) return;
    let latitude = data.latitude;
    let longitude = data.longitude;
    // show a marker on the map
    L.marker({lon: longitude, lat: latitude}).bindPopup('Current position').addTo(map);
}
