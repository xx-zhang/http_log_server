local payload = [[
{
    "upstream": "127.0.0.1:8080",
    "service_id": "",
    "latency": 64.99981880188,
    "response": {
        "headers": {
        "x-apisix-to-waf-error": "timeout",
        "x-apisix-upstream-status": "200",
        "content-type": "text/html; charset=utf-8",
        "x-coraza-uuid": "f324acbc-9a69-4710-b3de-afe23a8c6c72",
        "date": "Tue, 24 Dec 2024 09:24:46 GMT",
        "server": "APISIX/3.11.0",
        "x-apisix-to-waf": "yes",
        "content-length": "5",
        "x-apisix-to-waf-time": "64",
        "connection": "close"
        },
        "status": 200,
        "size": 331
    },
    "request": {
        "querystring": {},
        "uri": "/demo",
        "size": 82,
        "headers": {
        "host": "localhost:9080",
        "x-coraza-uuid": "f324acbc-9a69-4710-b3de-afe23a8c6c72",
        "user-agent": "curl/7.61.1",
        "accept": "*/*"
        },
        "method": "GET",
        "url": "http://localhost:9080/demo"
    },
    "apisix_latency": 63.99981880188,
    "client_ip": "::1",
    "server": {
        "version": "3.11.0",
        "hostname": "VM-217-149-tencentos"
    },
    "route_id": "example",
    "start_time": 1735032286048,
    "upstream_latency": 1
    }
]]

wrk.method = "POST"
-- wrk.path = "/"
-- wrk.headers["Cookie"] = "BEKY=123"
-- wrk.headers["CEKY"] = "123"
wrk.headers["Referer"] = "https://baidu.com"
wrk.headers["Connection"] = "close"
wrk.headers["Accept"] = "*/*"
wrk.headers["Content-Type"] = "application/json"
-- wrk.headers["X-Forwarded-For"] = "111.111.121.121"
wrk.body = payload


local counter = 1
local threads = {}

function setup(thread)
   thread:set("id", counter)
   table.insert(threads, thread)
   counter = counter + 1
end

function init(args)
   respBlock = 0
   respSuccess = 0
   respLimit = 0
   respError = 0
end

function request()
    return wrk.format()
end

function response(status, headers, body)
        if status ==624 then
                respBlock = respBlock + 1
	if status == 429 then
		respLimit = respLimit +1 
        elseif status ==403 then
                respBlock = respBlock + 1
        elseif status ==600 then
                respSuccess = respSuccess + 1
        elseif status ==200 then
                respSuccess = respSuccess + 1
        else
            respError = respError + 1
        end
end

function done(summary, latency, requests)
   respBlockCount = 0
   respErrorCount = 0
   respSuccessCount = 0
   respLimitCount = 0
   for index, thread in ipairs(threads) do
      local id        = thread:get("id")
      local respBlock  = thread:get("respBlock")
      local respError  = thread:get("respError")
      local respSuccess = thread:get("respSuccess")
      local respLimit = thread:get("respLimit")
      local msg = "thread %d made %d respError and got %d respSuccess and %d respBlockCount and %d respLimitCount"
      print(msg:format(id, respError, respSuccess, respBlockCount, respLimitCount))
      respBlockCount = respBlockCount + respBlock
      respErrorCount = respErrorCount + respError
      respSuccessCount = respSuccessCount + respSuccess
      respLimitCount = respLimitCount + respLimit
   end
   print("respErrorCount: ", respErrorCount)
   print("respSuccessCount: ", respSuccessCount)
   print("respBlockCount: ", respBlockCount)
  print("respBlockCount: ", respBlockCount)
end
