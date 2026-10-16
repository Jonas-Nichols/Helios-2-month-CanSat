# uv run fastapi dev
# use Websockets
from fastapi import FastAPI, WebSocket
import asyncio
import serial_asyncio_fast


# # set up serial
async def serial_read(device, **kwargs):
    reader, writer = await serial_asyncio_fast.open_serial_connection(url='COM8')
    while True:
        line = await reader.readline()
        print(line.decode('ascii').strip())

# start up server
app = FastAPI()

@app.get("/")
async def get():
    return {"message": "Hello World"}

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    print('connection made!!!!!!!!!!!!!!!!!!!!!!!!!!')
    await websocket.send_json([1,1234,34,1,0,400,30,3,34,34,5,0,0,0,4,2,8,0,0])

    while True:
        data = await websocket.receive_text()
        await serial_read(1)
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