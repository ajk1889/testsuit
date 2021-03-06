# Test-suit
A highly extensible HTTP Test server for linux built on C++. 

Test-suit runs predefined OS commands when it receives HTTP requests to URL paths defined in [urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json). This lets users add their own custom sscripts and use it with Testsuit. A detailed discussion on the best practices of reading HTTP Request data and returning result is [given here](#building-modules)

## How to use
### Starting Test-suit server
- Download and extract the latest release binary from [here](https://github.com/ajk1889/testsuit/releases "Test-suit binary releases")
- `cd` to the extracted location (or open a terminal in the extracted folder).
- Launch Test-suit by launching it directly `./testsuit`. Test-suit will run by default on port `1234`.

### Configuring Test-suit
Test-suit comes with several configuration options. Test-suit accepts configuration parameters as command line arguments as well as `stdin` input.
Test-suit expects the `command line arguments`/`stdin commands` to follow `key=value` format (example: `--maxdspeed=100`).
Single word arguments are also supported (example: `disable-log`).<br/> 
Here is a perfectly valid example for a Test-suit server launching command<br/>
`./testsuit --pingMs=3000 --maxdspeed=100 base-path='/home/user/Desktop' disable-log`

##### List of Test-suit's `command line arguments`
Key | Description | Possible values | Default value
--- | --- | --- | ---
`--pingMs` | The amount of time (in milliseconds) Test-suit should sleep after receiving an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) | 0 
`--maxdspeed` | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32) | 4294967296
`--maxuspeed` | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in conditions [specified here](#problems-with-inline-response)_ | Any number between 0 and 4294967296 (2^32) | 4294967296
`--port` | The port to which server should listen | 0-65536 (2^16), port should not be used by other processes | 1234
`--url-map` | Full path to url mapping file | A valid file path with or without quotes | [urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json) in the application directory
`--temp-dir` | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes | `/tmp/testsuit/`
`disable-log` | Single word argument to disable Test suit's request logging | _Not applicable_ | _Not applicable_
`disable-stdin` | Single word argument to disable accepting commands from `stdin`. If this parameter is passed while launching, [Test-suit's `stdin commands`](#list-of-test-suits-stdin-commands) will not work for that instance of testsuit server | _Not applicable_ | _Not applicable_

##### List of Test-suit's `stdin commands`
Key | Description | Possible values
--- | --- | ---
`pingMs` | The amount of time (in milliseconds) Test-suit should sleep after getting an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) 
`maxdspeed` | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32)
`maxuspeed` | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in conditions [specified here](#problems-with-inline-response)_ | Any number between 0 and 4294967296 (2^32)
`url-map` | Full path to url mapping file | A valid file path with or without quotes
`temp-dir` | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes
`logging` | Enable/disable Test suit's request logging | 0 or 1
`remap` | Single word command to reload urlMap.json file to memory | _Not applicable_
`stop` | Terminates Test-suit server | _Not applicable_

##### Note
All the command line arguments received by Test-suit is forwarded to its modules along with the HTTP request's data. 
So modules can have their own global command line arguments. For example, inbuilt module [list-files.py](https://github.com/ajk1889/testsuit/blob/master/modules/list-files.py) accepts `base-path` as a command line argument.

## Pre-packed modules
#### Files listing module
- This module lets clients browse through the file contents of the specified directory. The base directory can be specified either through command line argument (`--base-path=/path/to/the/folder/to/share`) or through `stdin` command `base-path=/path/to/the/folder/to/share`. File: [list-files.py](https://github.com/ajk1889/testsuit/blob/master/modules/list-files.py)
- Users can download/view files in the shared folder. Folders can be downloaded too as zip file. The zip will be generated at runtime and will be directly sent to the client instead of creating a zip file and then sharing it. This avoids processing delays and storage limitations on server when sharing a large folder. But this makes the download size to be undetermined. File will need to be downloaded from beginning if download was interrupted. File: [download.py](https://github.com/ajk1889/testsuit/blob/master/modules/download.py)
- Users can upload any file/folder to path they specify. The folder structure will be retained in the destination directory in server. File: [upload.py](https://github.com/ajk1889/testsuit/blob/master/modules/upload.py)
#### File Generator module
This module can be used to download a file of arbitary length. The contents of the file will be the following pattern of numbers.
```
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16....
```
The module supports partial content, so download can be resumed if interrupted. A more detailed description [can be found here](https://github.com/ajk1889/number-generator)

## Building modules
### Concept of modules in Test-suit
Upon receiving an HTTP request, test-suit server iterates through the contents of URL mapping file ([urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json)) until it finds a `regex` that matches the request path.
It then runs the OS command corresponding to the matched `regex` as a forked process and writes the full information about the request and Test-suit's global parameters to its `stdin`.
The forked process's input data will be encoded as `json` ([Data format specification](#input-data-to-modules)). 
The process is expected to write result in a format defined [here](#expected-response-from-modules) to its `stdout`.

An OS command that accepts input from Test-suit and return a response in valid format ([defined here](#expected-response-from-modules)) is called a module. Modules are free to ignore the response `json` written to their `stdin` if is not required for their operation.

This architecture allows programmers to write custom modules that execute tasks of their requirement and unloads all the low level aspects of networking to the kernel.
Building a module for Test-suit is simple. It is discussed [here](#building-modules)

**Note:** 
- [urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json) is not read every time Test-suit receives an HTTP request. Loads url-map to memory during following cases
  - Test-suit initialization (when program is started)
  - `url-map` path update using `stdin command`
  - Issue of `remap` `stdin command`
- The `POST` data of some HTTP requests may be too large to be handled in memory. In such cases, Test-suit will write the data to a temporary file and the file's absolute path will be included in `json` written to `stdin` instead of full post data.

### Input data to modules
The data written to `stdin` of process (executed OS command) will be a single line minified `json` string.
The string will be followed by a `new line character` (`\n`) so that most programs flushes their `stdin`. 
i.e. python interpreter will wait (and store input data to buffer) until its data written to its `stdin` encounters a `new line character` (`\n`).  

Here is a sample `request data json` written to process's `stdin`. 
_Content is formatted for readability; actual data will be minified_
```json
{
  "httpVersion": "HTTP/1.1",
  "path": "/search",
  "requestType": "POST",
  "GET": {
    "param1": ["value1"], 
    "param2": ["value for name='param2'", "value for second occurrence of name='param2'"],
    "param3": ["value3"]
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
    "key1": ["sample multi-line\r\ninput string"],
    "key2": ["sample single text string", "another text for same POST key ('key2')"]
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

Since there can be multiple values with same name in `GET` parameters, the `GET` object in the `json` supplied to `stdin` will be a `string` to `array of strings` map. 

As HTTP supports multiple `enctypes` for `POST` data, the type of `POST` `json object` may vary as follows.

##### application/x-www-form-urlencoded
If `Content-Type` field in HTTP request headers is `application/x-www-form-urlencoded`, the `POST` data in that request will be parsed and encoded to `json` in the following format.<br/>
The `POST` object will be a `string` to `array of strings` key value map, with both key and value **url decoded** in the `json`.
The `value` object is encoded as an `array of strings` to accommodate multiple values with same name in `POST` data<br/>
A sample `application/x-www-form-urlencoded` `POST` object will look like this
```json
{
  "key1": ["sample multi-line\r\ninput string"],
  "key2": ["sample single text string", "A sample 2nd value for 'key2'"]
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


### Expected response from modules
Modules are expected to print the result directly to its `stdout`. 
The expected output is essentially a single line minified json containing metadata of the actual response, two `new line character`s (`\n`) and the actual raw response content (if any).
 
Here is a sample response metadata `json`. (_**Note:** the `json` is formatted for readability. Actual response should be single line_)
```json
{
  "responseCode": 200,
  "headers": {"Content-Type": ["text/html"]},
  "length": 1337,
  "data": "inline"
}
```

#### Explanation for keys in response metadata
- **responseCode**: A valid HTTP response code number to be sent to the client for this request.
- **headers**: A `string` to `string[]` mapping of header values to be sent to the client. 
 This field isn't mandatory. Default values will be supplied if `headers` field is blank or non-existent
- **length**: The expected length of the response. This field is mandatory if value of `data` key is `inline`.
- **data**: The value should be either `inline` to indicate that the full response content will be printed directly to `stdin`
 or a valid path to a file whose content should be treated as the response content.

#### Handling huge response data
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

##### A sample file response metadata
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

**Note:** When `data` is set to a file path, modules need not write anything to its `stdout` other than the metadata and 2 trailing `new line character`s (`\n`).
Any more data written to stdout will not be read by Test-suit. 

#### Problems with inline response
1. Test-suit uses a single interface ([StreamDescriptor](https://github.com/ajk1889/testsuit/blob/master/implementations/file/StreamDescriptor.h)) for reading and writing to streams. 
This applies to Processes (executed OS commands) as well as network sockets.
Since this interface is responsible for managing upload & download speed, inter-process communication is also affected by `maxuspeed` and `maxdspeed` configurations.
So, maximum download speed for a direct `inline` response from a process will be minimum of `maxuspeed` and `maxdspeed`.<br/>
This speed doesn't affect reading & writing files as it is done through a separate interface (`std::ifstream`).
2. Test-suit kernel is not capable of handling `Content-Range` for `inline` response. 
Modules will need to handle the `Range` header field themselves. 
Modules are free to ignore it and reply with normal `Content-Length` only response. But this leads to poor user experience

#### When to use `inline` mode?
- When the response content is not an existing file and is dynamically generated<br/>
For tasks such as direct downloading a folder by zipping it, response is dynamically generated.
The alternative method will be to zip the folder to a temp file upon receiving a request and return a file response.
But that approach has downsides like storage space limits and high ping time (may cause request timeouts).
So `inline` mode is preferred in such conditions.
- When the response data size is too small and response content is already in memory<br/>
If the size of response data is less and content is already in memory, 
`inline` method is preferred even if a file exists with same content.
This is because in-memory data transaction is faster than disk IO.

### Adding module to [urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json)
[urlMap.json](https://github.com/ajk1889/testsuit/blob/master/urlMap.json) is a file that maps HTTP request path to an OS command (module).
It contains a list of `mapping object`s. Its format will be described later in this section.
A `mapping object` may also contain the `stdin commands` that are acceptable for the module.<br/>
This is an example `mapping object`.
```json
{
  "path": "^/files/?.*",
  "allowedArgs": [{
    "arg": "base-path",
    "description": "Absolute path to the base shared directory"
  }],
  "command": [
    "python3",
    "modules/list-files.py"
  ]
}
```
`path` is a `regex` which will be tested for complete case-insensitive match with the HTTP request path.
Note that HTTP request path will not contain any GET parameters.

`allowedArgs` is a list of information about `stdin command`s that are acceptable for this module. 
Its contents are self explanatory in the given example.

`command` is a list representing the OS command to execute when an HTTP request matching `path` is received.
The first item of `command` list will be passed to [execvp](https://man7.org/linux/man-pages/man3/execvp.3p.html) as file to execute and the whole list will be passed as command line argument to the program.
Therefore, modules should not assume first entry in their command line arguments is the executing file name.
