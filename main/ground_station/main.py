# use Websockets
from fastapi import FastAPI, WebSocket

app = FastAPI()


@app.get("/")
async def get():
    return {"message": "Hello World"}

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    while True:
        data = await websocket.receive_text()
        await websocket.send_json("{'a':1}")

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