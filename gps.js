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
const pub = mqtt.connect('mqtt://'+mqtthost);

pub.on('connect', function(){
    console.log('mqtt ' + mqtthost + ' connected.')
});

pub.on('error', function(err){
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
            var gps = {};
            gps.latitude = Number(nmeaData.latitude);
            gps.longitude = Number(nmeaData.longiture);
            mqttclient.publish('gps', gps);
        }
        else
        {
            //console.log("do not exits GPRMC");
        }
    }catch(e)
    {
        // console.log(e);
    }
}
