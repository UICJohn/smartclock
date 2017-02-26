from model import *

if __name__ == "__main__":
  
  try:
    WeatherInfo.create_table()
  except:
    print "TABLES WeatherInfo ALREADY EXISTS!"

  try:
    Weather.create_table()
  except:
    print "TABLES Weather ALREADY EXISTS!"