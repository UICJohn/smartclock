from model import *

if __name__ == "__main__":
  
  try:
    StartUpCheck.create_table()
  except:
    print "TABLES ALREADY EXISTS!"