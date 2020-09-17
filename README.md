# testsuit
A highly configurable HTTP Test server built on C++

This server runs custom OS commands (Defined in `urlMap.json` file) when it receives an HTTP request and returns the result to the client.

Upon Receiving an HTTP Request that matches path defined in the URL mapping file, the OS command will be executed and a JSON data of the following format will be written to its stdin.
### Request format
Formatted for readability, actual data will be minified.
```
{
    "GET":{
      "param1":"value1",
      "param2":"value2",
      "param3":"value3"
    },
    "HEADERS":{
      "Accept":["text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"],
      "Accept-Encoding":["gzip, deflate, br"],
      "Accept-Language":["en-US,en;q=0.9"],
      "Cache-Control":["max-age=0"],
      "Connection":["keep-alive"],
      "Content-Length":["70"],
      "Content-Type":["application/x-www-form-urlencoded"],
      "Host":["localhost:1234"],
      "Origin":["null"],
      "Sec-Fetch-Dest":["document"],
      "Sec-Fetch-Mode":["navigate"],
      "Sec-Fetch-Site":["cross-site"],
      "Sec-Fetch-User":["?1"],
      "Upgrade-Insecure-Requests":["1"],
      "User-Agent":["Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.83 Safari/537.36"]
    },
    "POST":{
      "key1":"sample single text string",
      "key2":"sample multiline\r\ninput string"
    },
    "applicationParams":{
      "additionalKwargs":{
        "files-list:basePath": "/home/"
      },
      "loggingAllowed":true,
      "maxDownloadSpeed":4294967295,
      "maxUploadSpeed":4294967295,
      "parallelConnections":10,
      "pingMs":0,
      "port":1234,
      "tempDir":"/tmp/testsuit",
      "urlMapFile":"/home/ajk/CLionProjects/testsuit/urlMap.json"
    },
    "httpVersion":"HTTP/1.1",
    "path":"/search",
    "requestType":"POST"
  }
  ```

### Expected response format
The executing OS command is expectued to output a JSON of following format to stdout.
```
{
  "responseCode": 200,
  "headers": {"Content-Type": ["text/html"]},
  "length": 1337,
  "data": "inline"
}
```
Followed by 2 next line character (`\n`) and the data to be delivered to client. OS commands are free to write the content to any required file and share the file to the client by setting `"data": "/absolute/path/to/file"`
explanation for all keys will be added later.

