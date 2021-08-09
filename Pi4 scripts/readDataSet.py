import influxdb
client = influxdb.InfluxDBClient(host='localhost', port = 8086)
client.switch_database('tomatoTest')
query = 'SELECT "CO2" FROM "Climate" WHERE ("location" = "Huey") AND time >= now() - 6h'


aa = client.query(query, chunked = True)
res = aa["CO2"]
for d in aa.get_points():
    print(d)
print(res)