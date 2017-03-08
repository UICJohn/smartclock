#!/usr/bin/env python
import sys
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from temboo.Library.Yahoo.Weather import GetWeatherByAddress
from temboo.core.session import TembooSession
import json
from model import WeatherInfo as wi
from model import Weather as w
from model import Forecast as f
from bridgeclient import BridgeClient as bridgeclient
from datetime import datetime

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
forecasts = res["channel"]["item"]["yweather:forecast"]
current= w.create(code = getWeatherByAddressResults.get_ForecastCode(),  temperature = getWeatherByAddressResults.get_Temperature(),text= getWeatherByAddressResults.get_ForecastText(), humidity = getWeatherByAddressResults.get_Humidity())

weathers = {}
counter = 0
for forecast in forecasts:
  weathers[counter] = f.create(code= forecast["@code"], high= forecast["@high"], low = forecast["@low"], text = forecast["@text"] )
  wi.create_or_get(code = forecast["@code"], body = forecast["@text"])
  counter += 1
counter = 0



bc = bridgeclient()
bc.begin()
print "bc.begin"
print "code = " + str(current.code) + "text = " + current.text
bc.mailbox("temperature:" + current.temperature + "," + weathers[0].high + "," + weathers[0].low)
bc.mailbox("humidity:" + getWeatherByAddressResults.get_Humidity())
weather_info = wi.get_or_create
bc.mailbox("temp_image:/root/images/medium/" + (wi.get_or_create(code = current.code, body= current.text))[0].image_path)
print (wi.get_or_create(code=current.code, body = current.text))[0].image_path
bc.mailbox("future:"+ weathers[1].high+","+weathers[2].high+","+weathers[3].high)
for index in weathers:
  if(index < 3):
    bc.mailbox("forcast_image"+str(index+1)+":/root/images/small/"+ (wi.get_or_create(code = weathers[index].code, body=weathers[index].text))[0].image_path)

bc.close()
print "bc.close"