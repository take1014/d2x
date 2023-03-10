var map = L.map('map',{
                    preferCanvas:true, //trueとし、Canvasレンダラーを選択
                }).setZoom(18);
//L.tileLayer('http://192.168.3.4/hot/{z}/{x}/{y}.png', {
L.tileLayer('http://192.168.3.4/hot/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
    maxZoom: 21,
    maxNativeZoom: 20
}).addTo(map);

//let mqttSub = mqtt.connect('ws://pi-ucsk.local:15675');
let mqttSub = mqtt.connect('ws://ucsk.local:15675');
mqttSub.on('connect', function() {
    mqttSub.subscribe('gps/ucsk');
});

var addMarker = null;
var oldData = {};
mqttSub.on('message', function(topic, message) {
    readGPSData(message.toString());
});

function readGPSData(data)
{
    // console.log(data);
    if(data == null) return;
    jsonData = JSON.parse(data);

    // Latitude  lines run east-west and are parallel to each other. If you go north, latitude values increase. Finally, latitude values (Y-values) range between -90 and +90 degrees.
    // Longitude lines run north-south. They converge at the poles. And its X-coordinates are between -180 and +180 degrees.
    if(jsonData.GPS_OUTPUT.longitude == NaN || jsonData.GPS_OUTPUT.latitude == NaN) return;
    if(jsonData.GPS_OUTPUT.longitude < -180 || jsonData.GPS_OUTPUT.longitude > 180) return;
    if(jsonData.GPS_OUTPUT.latitude  < -90  || jsonData.GPS_OUTPUT.latitude  > 90)  return;

    if (map && addMarker){
        map.removeLayer(addMarker);
        // points list -> [latitude, longitude]
        L.polyline([[jsonData.GPS_OUTPUT.latitude, jsonData.GPS_OUTPUT.longitude], [oldData.GPS_OUTPUT.latitude, oldData.GPS_OUTPUT.longitude]], {"color": "green", "weight": 5, "opacity": 0.3}).addTo(map);
    }
    // show a marker on the map marker points -> [longitude, latitude]
    addMarker = L.marker({lon: jsonData.GPS_OUTPUT.longitude, lat: jsonData.GPS_OUTPUT.latitude}).bindPopup('Current position').addTo(map);

    // map center -> marker position.
    map.setView({lon: jsonData.GPS_OUTPUT.longitude, lat: jsonData.GPS_OUTPUT.latitude}, map.getZoom());

    oldData = jsonData;
}