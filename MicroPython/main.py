import network
import machine
import time, math
import sys
import ujson
from machine import Pin, I2C
from Thermostat import BME280
import uasyncio as asyncio
import sys
from Thermostat import HTU21D
import uwebsockets.client
from Thermostat import constant
#from Thermostat import ssd1306



class DeviceSettings:
    def __init__(self, sn, target_temp, mode, running):
        self.SN = sn
        self.Target = target_temp
        self.Scale = "CELSIUS"
        self.Mode = mode
        self.Running = running
    def __repr__(self):
        return repr((self.SN, self.Target, self.Mode, self.Running))
    def __str__(self):
        return "Thermostat SN:" + self.SN
    def process_server_data(self, json_data):
        try:
            server_data = ujson.loads(json_data)
            temp_sn = server_data.get("sn")
            if temp_sn == self.SN:
                self.Mode = server_data.get("mode")
                self.Target = float(server_data.get("temp"))
                self.Scale = server_data.get("scale")
                print("Target temp: %.2f %s" % (self.Target, self.Scale))
        except OSError:
            print(OSError)


device = DeviceSettings(constant.serial_number, 22, "heat", False) 
serial_number = "1234567890"
running = False
mode = "heat"



if machine.reset_cause() == machine.DEEPSLEEP_RESET:
    print('woke from a deep sleep')
else:
    print('power on or hard reset') 



if sys.platform == 'pyboard':
    i2c = I2C(1)  # scl=X9 sda=X10
else:
    # Specify pullup: on my ESP32 board pullup resistors are not fitted :-(
    scl_pin = Pin(5, pull=Pin.PULL_UP, mode=Pin.OPEN_DRAIN)
    sda_pin = Pin(4, pull=Pin.PULL_UP, mode=Pin.OPEN_DRAIN)
    # Standard port
    i2c = I2C(-1, scl=scl_pin, sda=sda_pin)
    # Loboris port (soon this special treatment won't be needed).
    # https://forum.micropython.org/viewtopic.php?f=18&t=3553&start=390
    #i2c = I2C(scl=scl_pin, sda=sda_pin)


led = Pin(2, Pin.OUT)
led.value(1)

# for BME280
# ESP32
# i2c = I2C(scl=Pin(22), sda=Pin(21), freq=10000)
# ESP8266
#i2c = I2C(scl=Pin(5), sda=Pin(4), freq=10000)



# load settings #####################################################
f = open('settings.json')
settings_str = f.read()
f.close()
settings = ujson.loads(settings_str)
#####################################################################



# list all networks and connect to a saved network #################################
class TempNet:
    def __init__(self, name, signal):
        self.Name = name
        self.Signal = signal
    def __repr__(self):
        return repr((self.name, self.signal))
    def __str__(self):
        return self.Name + " (" + str(self.Signal) + ")"


wlan = network.WLAN(network.STA_IF)
wlan.active(True)


print("Networks available:")
results = wlan.scan()
found_networks = [TempNet(str(r[0]).strip('b\''), r[3]) for r in results]
found_networks.sort(key=lambda net: net.Signal, reverse=True) 

for p in found_networks:
    print(p)
print("")

print("Network active: " + str(wlan.active()))

found = False
for found_network in found_networks:
    if found:
        break
    for saved_net in settings.get("networks"):
        if saved_net.get("ssid") == found_network.Name:
            sys.stdout.write("Connecting to " + saved_net.get("ssid"))
            wlan.connect(saved_net.get("ssid") , saved_net.get("pass") ) 
            found = True
            break

while not wlan.isconnected():
    led.value(0)
    time.sleep_ms(100)
    led.value(1)
    time.sleep_ms(800)
    sys.stdout.write('.')
    pass
print('\nConnected, network config:', wlan.ifconfig())
print("")
########################################################################################3

htu = HTU21D.HTU21D(i2c, read_delay=2)  # read_delay=2 for test purposes


async def main():
    await htu

    uri = "ws://api.homejinnee.com/"
    websocket = uwebsockets.client.connect(uri)
    print("Connecting to {}:".format(uri))
    
    mesg = '{ "sn" : "%s", "event" : "CONNECTED" }' % (serial_number)
    websocket.send(mesg)
    initial_msg = websocket.recv()
    device.process_server_data(initial_msg)
    #print("RESPONSE: " + initial_msg)
    time.sleep_ms(100)
    

    while True:
        values = '{ "sn" : "%s", "temp" : "%.2f", "hum" : "%.2f", "mode" : "%s", "running" : "%s" }' % (serial_number, htu.temperature, htu.humidity, mode, running)
        
        #websocket = uwebsockets.client.connect(uri)
        websocket.send(values)
        print("SENDING: " + values)

        server_resp = websocket.recv()
        if server_resp != "":
            print("RECEIVED: " + server_resp)
            device.process_server_data(server_resp)
            
            #fstr = 'Temp {:5.1f} Humidity {:5.1f}'
            #print(fstr.format(htu.temperature, htu.humidity))

            led.value(0)
            time.sleep_ms(100)
            led.value(1)
            #time.sleep_ms(800)

            rtc = machine.RTC()
            rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

            #  DEEP SLEEP
            # set RTC.ALARM0 to fire after 10 seconds (waking the device)
            #rtc.alarm(rtc.ALARM0, 10000)
            #print("deepsleep...")
            #machine.deepsleep()
            #time.sleep_ms(100)

            # read values
            #try:
                #bme = BME280.BME280(i2c=i2c)
                #temp = bme.temperature
                #hum = bme.humidity
                #pres = bme.pressure
                #print('Temperature: ', temp)
            #except OSError:
                #print("Temperature: error!")

            #if server breaks connection, sleep and reconnect
        elif server_resp == "":
            websocket.close()
            time.sleep(5)
            websocket = uwebsockets.client.connect(uri)
            websocket.send(mesg)

        await asyncio.sleep(5)

loop = asyncio.get_event_loop()
loop.create_task(main())
loop.run_forever()
