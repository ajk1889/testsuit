# Test-suit
A highly configurable HTTP Test server for linux built on C++.

## How to use
### Starting Test-suit
- Download and extract the latest release binary from [here](https://github.com/ajk1889/testsuit/releases "Test-suit binary releases")
- `cd` to the extracted location (or open a terminal in the extracted folder).
- Launch Test-suit by launching it directly `./testsuit`. Test-suit will run by default on port `1234`.

### Configuring Test-suit
Test-suit comes with several configuration options. Test-suit accepts configuration parameters as command line arguments as well as `stdin` input.
Test-suit expects the command line/`stdin` arguments to follow this format `key=value` (example: `--maxdspeed=100`).
Single word arguments are also supported (example: `disable-log`).<br/> 
Here is a perfectly valid Test-suit launching command<br/>
`./testsuit --pingMs=3000 --maxdspeed=100 base-path='/home/user/Desktop' disable-log`

##### List of Test-suit's `command line arguments`
Key | Description | Possible values | Default value
--- | --- | --- | ---
--pingMs | The amount of time (in milliseconds) Test-suit should sleep after getting an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) | 0 
--maxdspeed | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32) | 4294967296
--maxuspeed | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in conditions [specified here](#descriptor-response-anomalies)_ | Any number between 0 and 4294967296 (2^32) | 4294967296
--port | The port to which server should listen | 0-65536 (2^16), port should not be used by other processes | 1234
--url-map | Full path to url mapping file | A valid file path with or without quotes | `urlMap.json` in current working directory
--temp-dir | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes | `/tmp/testsuit`
disable-log | Single word argument to disable Test suit's request logging | _Not applicable_ | _Not applicable_

##### List of Test-suit's `stdin commands`
Key | Description | Possible values
--- | --- | ---
pingMs | The amount of time (in milliseconds) Test-suit should sleep after getting an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) 
maxdspeed | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32)
maxuspeed | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in conditions [specified here](#descriptor-response-anomalies)_ | Any number between 0 and 4294967296 (2^32)
url-map | Full path to url mapping file | A valid file path with or without quotes
temp-dir | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes
logging | Enable/disable Test suit's request logging | 0 or 1
remap | Single word command to reload urlMap.json file to memory | _Not applicable_
stop | Terminates Test-suit server | _Not applicable_

##### Note
All the command line arguments received by Test-suit is forwarded to its modules along with the HTTP request's data. 
So modules can have their own global command line arguments. For example, inbuilt module `list-files.py` accepts `base-path` as a command line argument.

## Building modules
### Concept of modules in Test-suit
Upon receiving an HTTP request, test-suit server iterates through the contents of URL mapping file (`urlMap.json`) until it finds a `regex` that matches the request path.
It then runs the OS command corresponding to the matched `regex` as a forked process and writes the full information about the request and Test-suit's global parameters to its `stdin`.
The forked process's input data will be encoded as `json` ([Data format specification](#input-data-to-modules)). 
The process is expected to write result in a format defined [here](#expected-os-command-stdout-output-format) to its `stdout`.

An OS command that accepts input from Test-suit and return a response in valid format ([defined here](#expected-os-command-stdout-output-format)) is called a module. Modules are free to ignore the response `json` written to their `stdin` if is not required for their operation.

This architecture allows programmers to write custom modules that execute tasks of their requirement and unloads all the low level aspects of networking to the kernel.
Building a module for Test-suit is simple. It is discussed [here](#how-to-build-a-test-suit-module)

**Note:** 
- `urlMap.json` is not read every time Test-suit receives an HTTP request. Loads url-map to memory during following cases
  - Test-suit initialization (when program is started)
  - `url-map` path update using `stdin command`
  - Issue of `remap` `stdin command`
- The `POST` data of some HTTP requests may be too large to be handled in memory. In such cases, Test-suit will write the data to a temporary file and the file's absolute path will be included in `json` written to `stdin` instead of full post data.

### Input data to modules
The data written to `stdin` of process (executed OS command) will be a single line minified `json` string.
The string will be followed by a new line character (`\n`) so that most programs flushes their `stdin`. 
i.e. python interpreter will wait (and store input data to buffer) until its data written to its `stdin` encounters a new line character.  

Here is a sample `request data json` written to process's `stdin`. 
_Content is formatted for readability; actual data will be minified_
```json
{
  "httpVersion": "HTTP/1.1",
  "path": "/search",
  "requestType": "POST",
  "GET": {
    "param1": "value1",
    "param2": "value2",
    "param3": "value3"
  },
  "HEADERS": {
    "Accept": ["text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"],
    "Accept-Encoding": ["gzip, deflate, br"],
    "Accept-Language": ["en-US,en;q=0.9"],
    "Cache-Control": ["max-age=0"],
    "Connection": ["keep-alive"],
    "Content-Length": ["70"],
    "Content-Type": ["application/x-www-form-urlencoded"],
    "Host": ["localhost:1234"],
    "Origin": ["null"],
    "Sec-Fetch-Dest": ["document"],
    "Sec-Fetch-Mode": ["navigate"],
    "Sec-Fetch-Site": ["cross-site"],
    "Sec-Fetch-User": ["?1"],
    "Upgrade-Insecure-Requests": ["1"],
    "User-Agent": ["Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.83 Safari/537.36"]
  },
  "POST": {
    "key1": "sample single text string",
    "key2": "sample multiline\r\ninput string"
  },
  "applicationParams": {
    "additionalKwargs": {
      "base-path": "/home/"
    },
    "loggingAllowed": true,
    "maxDownloadSpeed": 4294967295,
    "maxUploadSpeed": 4294967295,
    "parallelConnections": 10,
    "pingMs": 0,
    "port": 1234,
    "tempDir": "/tmp/testsuit",
    "urlMapFile": "/home/user/urlMap.json"
  }
}
```
> **Note:**: For easiness of communication, sub-objects are represented using dot operators and array indexing. 
> i.e. value of `applicationParams.additionalKwargs.base-path` is `/home/` in the above example
> similarly value of `HEADERS.Host[0]` is `localhost:1234` here.

Most of the keys in the above example are self explanatory. 

As already discussed, the input data will contain application parameters 
(key and value of `command line arguments` and `stdin commands` and values for default parameters) inside `applicationsParams` object.
Test-suit's own parameters will reside directly inside `applicationParams` and
parameters of its installed modules can be found inside the sub-object `applicationParams.additionalKwargs`.

`HEADERS` object is a `string` to `string[]` map to support multiple occurrences of same header field in the request.
i.e. if the same header key repeats in the HTTP request, the array will contain values of both they keys in headers. 
The order in which they appear in headers is preserved in the `json array`.

As HTTP supports multiple `enctypes` for `POST` data, the type of `POST` `json object` may vary as follows.

##### application/x-www-form-urlencoded
If `Content-Type` field in HTTP request headers is `application/x-www-form-urlencoded`, the `POST` data in that request will be parsed and encoded to `json` in the following format.<br/>
The `POST` object will be a `string` to `string` key value map, with both key and value url decoded.<br/>
A sample `application/x-www-form-urlencoded` `POST` object will look like this
```json
{
  "key1": "sample single text string",
  "key2": "sample multiline\r\ninput string"
}
```

##### multipart/form-data
The post data will be parsed as `multipart/form-data` if value of `Content-Type` in HTTP headers is something like<br/>
`multipart/form-data; boundary=----WebKitFormBoundary3gJk0Q552rEzGh3p`<br/>
In some cases, the contents of HTTP request may be too large and therefore Test-suit may write it to a temp file and include its path in the `json` data instead of full `POST` content.
Since each `POST` key in multi-part form data can contain its own headers, the headers will be included in `POST` `json` object as well.<br/>
Here is a sample `multipart/form-data` `POST` object.
```json
{
  "file": [{
    "data": {
      "data": "/tmp/testsuit/jy0868acx2",
      "isFile": true
    },
    "headers": {
      "Content-Disposition": ["form-data; name=\"file\"; filename=\"a.bin\""],
      "Content-Type": ["application/octet-stream"]
    }
  }],
  "folder": [{
      "data": {
        "data": "/tmp/testsuit/eve8nmoigs",
        "isFile": true
      },
      "headers": {
        "Content-Disposition": ["form-data; name=\"folder\"; filename=\"testsuit/testsuit\""],
        "Content-Type": ["application/octet-stream"]
      }
    },
    {
      "data": {
        "data": "[\n  {\n    \"path\": \"^/files/?.*\",\n    \"allowedArgs\": [\n      {\n        \"arg\": \"base-path\",\n        \"description\": \"Absolute path to the base shared directory\"\n      }\n    ],\n    \"command\": [\n      \"python3\",\n      \"modules/list-files.py\"\n    ]\n  },\n  {\n    \"path\": \"^/download/?.*\",\n    \"allowedArgs\": [],\n    \"command\": [\n      \"python3\",\n      \"modules/download.py\"\n    ]\n  },\n  {\n    \"path\": \"^/reflect/?\",\n    \"allowedArgs\": [],\n    \"command\": [\n      \"python3\",\n      \"-c\",\n      \"import json\\ninp=input()\\nprint(json.dumps({\\\"data\\\":\\\"inline\\\",\\\"length\\\": len(inp), \\\"headers\\\": {\\\"Content-Type\\\": [\\\"application/json\\\"]}})+'\\\\n\\\\n'+inp)\"\n    ]\n  },\n  {\n    \"path\": \"^/upload/?\",\n    \"allowedArgs\": [],\n    \"command\": [\n      \"python3\",\n      \"modules/upload.py\"\n    ]\n  },\n  {\n    \"path\": \"^/$\",\n    \"allowedArgs\": [],\n    \"command\": [\n      \"echo\",\n      \"{\\\"responseCode\\\": 302, \\\"headers\\\": {\\\"Location\\\": [\\\"/files/\\\"]}, \\\"length\\\": 23, \\\"data\\\": \\\"inline\\\"}\\n\\n<h1>File not found</h1>\"\n    ]\n  },\n  {\n    \"path\": \".*\",\n    \"allowedArgs\": [],\n    \"command\": [\n      \"echo\",\n      \"{\\\"responseCode\\\": 404, \\\"headers\\\": {\\\"Content-Type\\\": [\\\"text/html\\\"]}, \\\"length\\\": 23, \\\"data\\\": \\\"inline\\\"}\\n\\n<h1>File not found</h1>\"\n    ]\n  }\n]",
        "isFile": false
      },
      "headers": {
        "Content-Disposition": ["form-data; name=\"folder\"; filename=\"testsuit/urlMap.json\""],
        "Content-Type": ["application/json"]
      }
    },
    {
      "data": {
        "data": "/tmp/testsuit/z9fxqqqvjj",
        "isFile": true
      },
      "headers": {
        "Content-Disposition": ["form-data; name=\"folder\"; filename=\"testsuit/modules/upload.py\""],
        "Content-Type": ["text/x-python"]
      }
    }
  ],
  "path": [{
    "data": {
      "data": "/home/user/Desktop/",
      "isFile": false
    },
    "headers": {
      "Content-Disposition": ["form-data; name=\"path\""]
    }
  }]
}
```
 
Here `file`, `folder` and `path` are all keys in the POST data. 
For sake of simplicity, lets call objects corresponding to those keys as `form-data objects`. 
Every `POST` key can have more than one `form-data object`s. This lets Test-suit server support folder upload from browser. 
Here `POST.folder` contains 3 `form-data object`s.<br/>
###### About `form-data object`s
Every `form-data object` contains two keys: `data` representing the actual content of that key, and `headers` representing its metadata.

`data` key is guaranteed to contain two keys `data.data` and `data.isFile`. 
- For small request contents (less than 2048 bytes), `data.data` itself will hold the whole content. In that case, `data.isFile` will be false. 
- For large request contents, `data.data` hold the absolute path of the temp file to which the content was written. 
 To identify that the content should be read from a file, `data.isFile` will be true. 

##### text/plain
For `text/plain` `POST` requests, the data is read either to the memory or to a file, similar to `multipart/form-data`. 
The `POST` `json` is same as a single `POST.key[index].data` entry in encoding for [multipart/form-data](#multipartform-data).<br/>
A sample `text/plain` `POST` object will look like this
 ```json
{
  "data": "/tmp/testsuit/z9fxqqqvjj",
  "isFile": true
}
 ```


### Expected responses from modules
Modules are expected to print the result directly to its `stdout`. 
The expected output is essentially a single line minified json containing metadata of the actual response, two `next line character`s (`\n`) and the actual raw response content (if any).
 
Here is a sample response metadata `json`. (_**Note:** the `json` is formatted for readability. Actual response should be single line_)
```json
{
  "responseCode": 200,
  "headers": {"Content-Type": ["text/html"]},
  "length": 1337,
  "data": "inline"
}
```

##### Explanation for keys in response metadata
- **responseCode**: A valid HTTP response code number to be sent to the client for this request.
- **headers**: A `string` to `string[]` mapping of header values to be sent to the client. 
 This field isn't mandatory. Default values will be supplied if `headers` field is blank or non-existent
- **length**: The expected length of the response. This field is mandatory if value of `data` key is `inline`.
- **data**: The value should be either `inline` to indicate that the full response content will be printed directly to `stdin`
 or a valid path to a file whose content should be treated as the response content.

##### Handling huge response data
For sending responses like a contents of an existing file, `data` field can be used to point the file path 
instead setting `"data": "inline"` and writing contents directly to process's `stdout`. 
A notable advantage of this approach is that Test-suit server itself manages `partial content` responses.
For Test-suit to manage partial data response, the process is expected to do the following.
- Set `"responseCode": 206` (to adhere to HTTP standards) in metadata
- Add an additional key `offset` indicating the byte offset from which the file should be read. The byte at position `offset` is included in response.
- Optionally, add an additional key `limit` indicating the index of last byte to be read in the file (**inclusive**). 
 If `limit` is not provided, or if `limit` is set to 0, the file will be read until end.
- Set `Content-Type` and `Content-Disposition` header parameters if required.
 Note that modules are **NOT** required to add `Content-Range` in the response header metadata object when using `offset` and `limit` keys.

###### A sample file response metadata
_**Note:** the `json` is formatted for readability. Actual response should be single line_
```json
{
  "responseCode": 206,
  "headers": {
    "Content-Type": [
      "application/octet-stream"
    ],
    "Content-Disposition": [
      "attachment; filename=\"file.pdf\""
    ]
  },
  "data": "/home/user/file.pdf",
  "offset": 166667
}
```

**Note:** When `data` is set to a file path, modules need not write anything to its `stdout` other than the metadata and 2 trailing `next line character`s (`\n`).
Any more data written to stdout will not be read by Test-suit. 