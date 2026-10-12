// Create a line chart with values
function lineChart(elemCanvas,valX,valY,title,titleY,titleX,){
  return new Chart(elemCanvas, {
    type: 'line',
    defaults: {
      elements: {
        point: {
          radius: 0
        }
      },
    },
    options: {
      plugins: {
        tooltip: {
          enabled: false
        }
      }
    },
    data: {
      labels: valX,
      datasets: [{
        label: title,
        data: valY,
        borderWidth: 1,
        borderColor: "rgba(30,30,30,1)",
        backgroundColor: "rgba(30,30,30,1)"
      }]
    },
    options: {
      scales: {
        y: {
          title: {
            display: true,
            text: titleY,
          },
          beginAtZero: true
        },
        x: {
          title: {
            display: true,
            text: titleX,
          }
        }
      }
    }
  });
}


const altctx = document.getElementById('altitude');
const tempctx = document.getElementById('temperature');
const batVoltctx = document.getElementById('batteryVoltage');

let altY = [0,10,30,39,46,50,63];
let altT = [0,0.26,0.50,0.78,1.03,1.30,1.56];

let tempY = [];
let tempT = [];

let batVoltY = [];
let batVoltT = [];

lineChart(altctx,altT,altY,'Altitude','Altitude (m)','Time (seconds)');
lineChart(tempctx,tempT,tempY,'Temperature','Temperature (ºC)','Time (s)');
lineChart(batVoltctx,batVoltT,batVoltY,'Battery Voltage','Voltage (v)','Time (s)');


let CanSatModel = document.getElementById('CanSatModel');

// Leaflet
// Center the map on London coordinates [latitude, longitude] with a zoom level of 13
import {LeafletMap, TileLayer, Marker, Polyline} from 'leaflet';
let map = new LeafletMap('map', {
  center: [34.900703, -86.615140],
    zoom: 15,
    zoomControl: false,
    dragging: false
});
new TileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(map);
// new Marker(coordinates).addto(map);


const timeVal = document.getElementById('timeVal');
const P1Val = document.getElementById('P1Val');
const P2Val = document.getElementById('P2Val');
const accelXVal = document.getElementById('accelXVal');
const accelYVal = document.getElementById('accelYVal');
const accelZVal = document.getElementById('accelZVal');
const packetCountVal = document.getElementById('packetCountVal');
const latVal = document.getElementById('latVal');
const longVal = document.getElementById('longVal');
const satsUsedVal = document.getElementById('satsUsedVal');


// Set up WebSocket connection at probabable address
var connection = new WebSocket("ws://localhost:8000/ws");
// ws incoming data
connection.onmessage = function(event) {
  // event.data
  // simple displayed data
  timeVal.innerHTML = event.data['timeElapsed'];
  P1Val.innerHTML = event.data['panelAVolt'];
  P2Val.innerHTML = event.data['panelBVolt'];
  accelXVal.innerHTML = event.data['accelerationX'];
  accelYVal.innerHTML = event.data['accelerationY'];
  accelZVal.innerHTML = event.data['accelerationZ'];
  packetCountVal.innerHTML = event.data['packetCount'];
  latVal.innerHTML = event.data['latitude'];
  longVal.innerHTML = event.data['longitude'];
  satsUsedVal.innerHTML = event.data['satsUsed'];
}
  // char teamId[4];        //
  // int timeElapsed;       //
  // int packetCount;       //
  // int curStage;
  // uint8_t mechState;
  // float altitude;
  // int temperature;
  // float batteryVoltage;  
  // float latitude;        ..
  // float longitude;       ..
  // int satsUsed;          //
  // float gyroX;           
  // float gyroY;           
  // float gyroZ;           
  // float accelerationX;   //
  // float accelerationY;   //
  // float accelerationZ;   //
  // float panelAVolt;      //
  // float panelBVolt;      //

// ws outgoing data
function sendCommand(command){
  console.log(command);
  ws.send(command)
}

// button links
document.getElementById("releaseBut").addEventListener("click",()=>{sendCommand('r')});
document.getElementById("extensionBut").addEventListener("click",()=>{sendCommand('e')});

document.getElementById("s0But").addEventListener("click",()=>{sendCommand('s0')});
document.getElementById("s1But").addEventListener("click",()=>{sendCommand('s1')});
document.getElementById("s2But").addEventListener("click",()=>{sendCommand('s2')});
document.getElementById("s3But").addEventListener("click",()=>{sendCommand('s3')});
document.getElementById("s4But").addEventListener("click",()=>{sendCommand('s4')});