var map = L.map('map').setZoom(18);
L.tileLayer('http://192.168.3.32/hot/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
    maxZoom: 21,
    maxNativeZoom: 20
}).addTo(map);

let mqttSub = mqtt.connect('ws://pi-ucsk.local:15675');
mqttSub.on('connect', function() {
    mqttSub.subscribe('gps');
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
    if(jsonData.GPRMC.longitude == NaN || jsonData.GPRMC.latitude == NaN) return;
    if(jsonData.GPRMC.longitude < -180 || jsonData.GPRMC.longitude > 180) return;
    if(jsonData.GPRMC.latitude  < -90  || jsonData.GPRMC.latitude  > 90)  return;

    if (map && addMarker){
        map.removeLayer(addMarker);
        // points list -> [latitude, longitude]
        L.polyline([[jsonData.GPRMC.latitude, jsonData.GPRMC.longitude], [oldData.GPRMC.latitude, oldData.GPRMC.longitude]], {"color": "green", "weight": 5, "opacity": 0.3}).addTo(map);
    }
    // show a marker on the map marker points -> [longitude, latitude]
    addMarker = L.marker({lon: jsonData.GPRMC.longitude, lat: jsonData.GPRMC.latitude}).bindPopup('Current position').addTo(map);

    // map center -> marker position.
    map.setView({lon: jsonData.GPRMC.longitude, lat: jsonData.GPRMC.latitude}, map.getZoom());

    oldData = jsonData;
}

