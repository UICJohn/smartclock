from peewee import *
import sys
from datetime import datetime

db = SqliteDatabase('/root/smartclock.db')

class Base(Model):
  class Meta:
    database = db

  def all():
    select()

class WeatherInfo(Base):
  code = IntegerField()
  body = CharField()
  image_path = CharField()

class Weather(Base):
  code = IntegerField()
  high = IntegerField()
  low = IntegerField()
  text = CharField()
  created_at = DateTimeField(default = datetime.now().strftime('%Y-%m-%d %H:%M'), constraints=[SQL('DEFAULT CURRENT_TIMESTAMP')])