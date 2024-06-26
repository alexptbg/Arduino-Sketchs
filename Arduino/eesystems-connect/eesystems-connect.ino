wifi.setmode(wifi.STATION)
station_cfg={ssid="EESYSTEMS",pwd="0894587672"}
station_cfg.save=true
wifi.sta.config(station_cfg)
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
conn:on("receive",function(conn,payload)
print(payload)
conn:send("<h1>This is a simple webserver</h1>")
conn:send("<h2>Hello</h2>")
conn:send("Hello, Lolin NodeMCU V3.")
end)
end)
