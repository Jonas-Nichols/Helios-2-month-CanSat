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

let altCt = lineChart(altctx,altT,altY,'Altitude','Altitude (m)','Time (seconds)');
let tempCt = lineChart(tempctx,tempT,tempY,'Temperature','Temperature (ºC)','Time (s)');
let batVoltCt = lineChart(batVoltctx,batVoltT,batVoltY,'Battery Voltage','Voltage (v)','Time (s)');

// get info for model-viewer
let CanSatModel = document.getElementById('CanSatModel');
let mechState = 0;
let curStage = 0;

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

// data placeholders
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

// stage buttons
const s0But = document.getElementById("s0But");
const s1But = document.getElementById("s1But");
const s2But = document.getElementById("s2But");
const s3But = document.getElementById("s3But");
const s4But = document.getElementById("s4But");


// Set up WebSocket connection at probabable address
var connection = new WebSocket("ws://localhost:8000/ws");
// changes background to white when connected
connection.addEventListener("open",() => {
  document.body.style.backgroundColor = '#ffffff';
});
// changes background to red when disconnected
connection.addEventListener("close", () => {
  document.body.style.backgroundColor = '#ffaaaa';
});

// ws incoming data
connection.onmessage = function(event) {
  let curVal = JSON.parse(event.data);

  // event.data
  // simple displayed data
  timeVal.innerHTML = curVal[1];
  P1Val.innerHTML = curVal[17];
  P2Val.innerHTML = curVal[18];
  accelXVal.innerHTML = curVal[14];
  accelYVal.innerHTML = curVal[15];
  accelZVal.innerHTML = curVal[16];
  packetCountVal.innerHTML = curVal[2];
  latVal.innerHTML = curVal[8];
  longVal.innerHTML = curVal[9];
  satsUsedVal.innerHTML = curVal[10];

  // add chart data
  altCt.data.labels.push(curVal[1]);
  altCt.data.datasets[0].data.push(curVal[6]);
  tempCt.data.labels.push(curVal[1]);
  tempCt.data.datasets[0].data.push(curVal[7]);
  batVoltCt.data.labels.push(curVal[1]);
  batVoltCt.data.datasets[0].data.push(curVal[5]);
  
  altCt.update();
  tempCt.update();
  batVoltCt.update();

  // current stage
  if (curVal[3] != curStage) {
    switch (curStage){
      case 0:
        s0But.classList.remove("active");
        break;
      case 1:
        s1But.classList.remove("active");
        break;
      case 2:
        s2But.classList.remove("active");
        break;
      case 3:
        s3But.classList.remove("active");
        break;
      case 4:
        s4But.classList.remove("active");
        break;
    }
    switch (curVal[3]) {
      case 0:
        s0But.classList.add("active");
        break;
      case 1:
        s1But.classList.add("active");
        break;
      case 2:
        s2But.classList.add("active");
        break;
      case 3:
        s3But.classList.add("active");
        break;
      case 4:
        s4But.classList.add("active");
        break;
    }
  }

  // orientation and mechstate display
  CanSatModel.orientation = `${curVal[11]}deg ${curVal[12]}deg ${curVal[13]}deg`;

}
  //0 char teamId[4];       //
  //1 int timeElapsed;      //
  //2 int packetCount;      //
  //3 int curStage;
  //4 uint8_t mechState;
  //5 float altitude;       //
  //6 int temperature;      //
  //7 float batteryVoltage; //
  //8 float latitude;        ..
  //9 float longitude;       ..
  //10 int satsUsed;          //
  //11 float gyroX;           
  //12 float gyroY;           
  //13 float gyroZ;           
  //14 float accelerationX;   //
  //15 float accelerationY;   //
  //16 float accelerationZ;   //
  //17 float panelAVolt;      //
  //18 float panelBVolt;      //

// ws outgoing data
function sendCommand(command){
  alert(command);
  ws.send(command);
  alert('sent');
}

// button links
document.getElementById("releaseBut").addEventListener("click",()=>{sendCommand('r')});
document.getElementById("extensionBut").addEventListener("click", () => {sendCommand('e')});

s0But.addEventListener("click",()=>{sendCommand('s0')});
s1But.addEventListener("click",()=>{sendCommand('s1')});
s2But.addEventListener("click",()=>{sendCommand('s2')});
s3But.addEventListener("click",()=>{sendCommand('s3')});
s4But.addEventListener("click",()=>{sendCommand('s4')});