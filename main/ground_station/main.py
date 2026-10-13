# use Websockets
from fastapi import FastAPI, WebSocket
import json

app = FastAPI()


@app.get("/")
async def get():
    return {"message": "Hello World"}

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    await websocket.send_json([1,1234,34,1,0,400,30,3,34,34,5,0,0,0,4,2,8,0,0])
    while True:
        data = await websocket.receive_text()
        print(data)
        await websocket.send_json('[1,1234,34,1,0,400,30,3,34,34,5,0,0,90,4,2,8,0,0]')

#   char teamId[4];
#   int timeElapsed;
#   int packetCount;
#   int curStage;
#   uint8_t mechState;
#   float altitude;
#   int temperature;
#   float batteryVoltage;
#   float latitude;
#   float longitude;
#   int satsUsed;
#   float gyroX;
#   float gyroY;
#   float gyroZ;
#   float accelerationX;
#   float accelerationY;
#   float accelerationZ;
#   float panelAVolt;
#   float panelBVolt;