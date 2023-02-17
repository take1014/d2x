// import modules
const serialport = require('serialport');
const nmea       = require('nmea-0183');
const mqtt       = require('mqtt');

// define parameters
var PORT = '/dev/ttyACM0';
var BAUDRATE = 4800;


// define serialport
const port = new serialport(PORT, {
    baudRate: parseInt(BAUDRATE),
    //parser: new serialport.parsers.Readline(readline)
    parser: new serialport.parsers.Readline("\r\n")
});

// open serial port
port.on('open', function(err){
    console.log('port open');
});


// mqtt modules
const mqtthost = 'localhost'
const mqttPub = mqtt.connect('mqtt://'+mqtthost);

mqttPub.on('connect', function(){
    console.log('mqtt ' + mqtthost + ' connected.')
});

mqttPub.on('error', function(err){
    console.log(err);
});

port.on('data', readData);

function readData(data)
{
    try
    {
        var nmeaData = nmea.parse(data.toString());
        if(nmeaData['id'] == 'GPRMC')
        {
            var gpsData = {};
            gpsData.id        = nmeaData['id'];
            gpsData.latitude  = Number(nmeaData.latitude);
            gpsData.longitude = Number(nmeaData.longitude);
            console.log(gpsData);
            mqttPub.publish('gps', JSON.stringify(gpsData));
        }
        else
        {
            // mqttPub.publish('gps', null);
        }
    }catch(e)
    {
        // mqttPub.publish('gps', null);
    }
}
