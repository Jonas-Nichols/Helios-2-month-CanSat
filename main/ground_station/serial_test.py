import asyncio
import serial_asyncio_fast


# set up serial
async def serial_read(device, **kwargs):
    reader, writer = await serial_asyncio_fast.open_serial_connection(url='COM8', baudrate=112700)
    while True:
        line = await reader.readline()
        print(line.decode('ascii').strip())

loop = asyncio.get_event_loop()
tasks = [
    serial_read('/dev/ttyACM0', baudrate=57600),
    serial_read('/dev/ttyUSB0', baudrate=9600, bytesize=7),
]
asyncio.gather(*tasks)
loop.run_forever()
loop.close()