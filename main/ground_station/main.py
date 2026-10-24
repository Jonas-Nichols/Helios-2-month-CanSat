# uv run fastapi dev
# use Websockets
import csv

from contextlib import asynccontextmanager
from fastapi import FastAPI, WebSocket
import asyncio
import serial_asyncio_fast

comPort = 'COM5'

# start up server   
app = FastAPI()


@app.get("/")
async def get():
    return "Wrong place man"

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    print('connection made!!!!!!!!!!!!!!!!!!!!!!!!!!')
    await websocket.send_json([1,1234,34,1,0,400,30,3,34,34,5,0,0,0,4,2,8,0,0])
    
    serial_reader, serial_writer = await serial_asyncio_fast.open_serial_connection(url=comPort,baudrate=115200,rtscts=False,dsrdtr=False)

    # reads from serial, writes to csv, sends to socket
    async def serial_read(serial_reader, websocket: WebSocket):

        with open(f'flight_data.csv', 'w') as csvfile:
            csvwriter = csv.writer(csvfile, dialect='excel')

            csvwriter.writerow(['Team Id','Time Elapsed', 'Packet Count', 'Cur Stage', 'Mech State', 'Altitude', 'temperature', 'BatteryVoltage', 'latitude', 'longitude','Satilites Used','Gyro X', 'Gyro Y', 'Gyro Z', 'Accel X', 'Accel Y', 'Accel Z', 'Panel A Voltage', 'Panel B Voltage'])

            while True:
                line = await serial_reader.readline()
                print(line.decode('ascii'))

                csvwriter.writerow(line.decode('ascii').strip('[]').split(','))

                await websocket.send_json(line.decode('ascii').strip('[]').split(','))

    # take commands from socket, write commands to serial
    async def serial_write(serial_writer, websocket: WebSocket):
        while True:

            data = await websocket.receive_text()
            print(data)
            serial_writer.write(data.encode('UTF-8'))

    await asyncio.gather(serial_read(serial_reader, websocket), serial_write(serial_writer, websocket))
    
    
    

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