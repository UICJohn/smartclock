import sys
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from temboo.Library.Yahoo.Weather import GetWeatherByAddress
from temboo.core.session import TembooSession
import json
from model import WeatherInfo as wi
from model import Weather as w
from bridgeclient import BridgeClient as bridgeclient

session = TembooSession("marsland", "SmartClock", "bkOW4wZpfNVuXC2A6f52z7Ja4oAukXZL")
getWeatherByAddressChoreo = GetWeatherByAddress(session)
getWeatherByAddressInputs = getWeatherByAddressChoreo.new_input_set()
getWeatherByAddressInputs.set_ResponseFormat("json")
getWeatherByAddressInputs.set_Address("1 beenleigh ave 5087 sa")
getWeatherByAddressInputs.set_Day("4")
getWeatherByAddressInputs.set_Units("c")
getWeatherByAddressResults = getWeatherByAddressChoreo.execute_with_results(getWeatherByAddressInputs)

# print("Response: " + getWeatherByAddressResults.get_Response())

res = json.loads(getWeatherByAddressResults.get_Response())
forcasts = res["channel"]["item"]["yweather:forecast"]
weathers = {}
counter = 0
for f in forcasts:
  weathers[counter] = w.create(code= f["@code"], high= f["@high"], low = f["@low"], text = f["@text"] )
  counter += 1

counter = 0

current_temperature = getWeatherByAddressResults.get_Temperature()

bc = bridgeclient()
bc.begin()
print "bc.begin"
bc.mailbox("temperature:" + current_temperature + "," + weathers[0].high + "," + weathers[0].low)
bc.mailbox("humidity:" + getWeatherByAddressResults.get_Humidity())
bc.mailbox("temp_image:/mnt/sda1/images/medium/clear_night.bmp")
bc.mailbox("future:"+ weathers[1].high+","+weathers[2].high+","+weathers[3].high)
bc.mailbox("forcast_image1:/mnt/sda1/images/small/cloud.bmp")
bc.mailbox("forcast_image2:/mnt/sda1/images/small/cloud.bmp")
bc.mailbox("forcast_image3:/mnt/sda1/images/small/cloud.bmp")
bc.close()
print "bc.close"