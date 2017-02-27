import json
import sys    
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from temboo.Library.Labs.GetWeather import ByAddress
from temboo.core.session import TembooSession
from model import Weather as w
from model import WeatherInfo as wi
from bridgeclient import BridgeClient as bridgeclient
# from mailbox import Mailbox as mailbox
# # Create a session with your Temboo account details
# session = TembooSession("marsland", "SmartClock", "bkOW4wZpfNVuXC2A6f52z7Ja4oAukXZL")

# # Instantiate the Choreo
# byAddressChoreo = ByAddress(session)

# # Get an InputSet object for the Choreo
# byAddressInputs = byAddressChoreo.new_input_set()

# # Set the Choreo inputs
# byAddressInputs.set_Address("1 beenleigh Ave 5087 SA")

# # Execute the Choreo
# byAddressResults = byAddressChoreo.execute_with_results(byAddressInputs)

# # Print the Choreo outputs
# json_weather = json.loads(byAddressResults.get_Response())

# weather = w.create(
#   title = json_weather["title"],
#   high_temperature  = (int(json_weather["high"])-32) * 5 / 9, 
#   low_temperature   = (int(json_weather["low"])-32) * 5 / 9,
#   current_temperature = (int(json_weather["temperature"])-32) * 5 / 9,
#   forecast = json_weather["text"]
# )

bc = bridgeclient()

# bc.put("temperature", (int(json_weather["temperature"]) - 32) * 5 / 9)
# bc.put("temperature", 22)
# bc.put("temp_image", "/mnt/sda1/images/medium/clear_night.bmp")
# bc.put("temp_change", 1)
bc.begin()
print "bc.begin"
bc.mailbox("temperature: 22,30,18")
bc.mailbox("humidity:50")
bc.mailbox("temp_image:/mnt/sda1/images/medium/clear_night.bmp")
bc.mailbox("future:33,23,33")
bc.mailbox("forcast_image1:/mnt/sda1/images/small/cloud.bmp")
bc.mailbox("forcast_image2:/mnt/sda1/images/small/cloud.bmp")
bc.mailbox("forcast_image3:/mnt/sda1/images/small/cloud.bmp")
bc.close()
print "bc.close"