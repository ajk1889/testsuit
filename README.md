# Test-suit
A highly configurable HTTP Test server for linux built on C++.

### How to use
##### Starting Test-suit
- Download and extract the latest release binary from [here](https://github.com/ajk1889/testsuit/releases "Test-suit binary releases")
- `cd` to the extracted location (or open a terminal in the extracted folder).
- Launch testsuit by launching it directly `./testsuit`. Test-suit will run by default on port `1234`.

##### Configuring Test-suit
testsuit comes with several configuration options. testsuit accepts configuration parameters as command line arguments as well as `stdin` input.
testsuit expects the command line/`stdin` arguments to follow this format `key=value` (example: `--maxdspeed=100`).
Single word arguments are also supported (example: `disable-log`).<br/> 
Here is a perfectly valid Test-suit launching command<br/>
`./testsuit --pingMs=3000 --maxdspeed=100 base-path='/home/user/Desktop' disable-log`

###### List of testsuit's own command line arguments
Key | Description | Possible values | Default value
--- | --- | --- | ---
--pingMs | The amount of time (in milliseconds) testsuit should sleep after getting an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) | 0 
--maxdspeed | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32) | 4294967296
--maxuspeed | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in some conditions [specified here](#descriptor-response-anomalies)_ | Any number between 0 and 4294967296 (2^32) | 4294967296
--port | The port to which server should listen | 0-65536 (2^16), port should not be used by other processes | 1234
--url-map | Full path to url mapping file | A valid file path with or without quotes | `urlMap.json` in current working directory
--temp-dir | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes | `/tmp/testsuit`
disable-log | Single word argument to disable Test suit's request logging | _Not applicable_ | _Not applicable_

###### List of testsuit's own `stdin` commands
Key | Description | Possible values
--- | --- | ---
pingMs | The amount of time (in milliseconds) testsuit should sleep after getting an HTTP request. OS command will be executed only after this sleep is completed. This simulates `ping`. | Any number between 0 and 4294967296 (2^32) 
maxdspeed | Approximate expected download speed (in KB/s) at the client's end. The actual download speed at clients end may be affected by network conditions. | Any number between 0 and 4294967296 (2^32)
maxuspeed | Approximate expected upload speed (in KB/s) at the client's end. The actual upload speed at clients end may be affected by network conditions. _**Note:** This may affect download speed in some conditions [specified here](#descriptor-response-anomalies)_ | Any number between 0 and 4294967296 (2^32)
url-map | Full path to url mapping file | A valid file path with or without quotes
temp-dir | Directory to store `POST` contents exceeding 2KB size | A valid file path with or without quotes
logging | Enable/disable Test suit's request logging | 0 or 1

###### Note
All the command line arguments received by testsuit is forwarded to its modules along with the HTTP request's data. 
So modules can have their own global command line arguments. For example, inbuilt module `list-files.py` accepts `base-path` as a command line argument.

## Building modules
### Concept of modules in Test-suit
Upon receiving an HTTP request, test-suit server iterates through the URL mapping file (`urlMap.json`) until it finds a `regex` that matches the request path.
It then runs the OS command corresponding to the matched `regex` as a forked process and writes the full information about the request and Test-suit's global parameters to its `stdin`.
The forked process's input data will be encoded as `json` ([Data format specification](#input-data-to-modules)). 
The process is expected to write result in a format defined [here](#expected-os-command-stdout-output-format) to its `stdout`.

An OS command that accepts input from Test-suit and return a response in valid format ([defined here](#expected-os-command-stdout-output-format)) is called a module. Modules are free to ignore the response `json` written to their `stdin` if is not required for their operation.

This architecture allows programmers to write custom modules that execute tasks of their requirement and unloads all the low level aspects of networking to the kernel.
Building a module for Test-suit is simple. It is discussed [here](#how-to-build-a-test-suit-module)

**Note:**  The `POST` data of some HTTP requests may be too large to be handled in memory. In such cases, Test-suit will write the data to a temporary file and the file's absolute path will be included in `json` written to `stdin` instead of full post data.

### Input data to modules
The data written to `stdin` of process (executed OS command) will be a single line minified `json` string.
The string will be followed by a new line character (`\n`) so that most programs flushes their `stdin`. 
i.e. python interpreter will wait (and store input data to buffer) until its data written to its `stdin` encounters a new line character.  

Here is a sample `request data json` written to process's `stdin`. 
_Content is formatted for readability; actual data will be minified_
```json
{
    "httpVersion":"HTTP/1.1",
    "path":"/search",
    "requestType":"POST",
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
        "base-path": "/home/"
      },
      "loggingAllowed":true,
      "maxDownloadSpeed":4294967295,
      "maxUploadSpeed":4294967295,
      "parallelConnections":10,
      "pingMs":0,
      "port":1234,
      "tempDir":"/tmp/testsuit",
      "urlMapFile":"/home/ajk/CLionProjects/testsuit/urlMap.json"
    }
  }
  ```

### Expected OS Command stdout output format
The executing OS command is expectued to output a JSON of following format to stdout.
```json
{
  "responseCode": 200,
  "headers": {"Content-Type": ["text/html"]},
  "length": 1337,
  "data": "inline"
}
```
Followed by 2 next line character (`\n`) and the data to be delivered to client. OS commands are free to write the content to any required file and share the file to the client by setting `"data": "/absolute/path/to/file"`

explanation for all keys will be added later.

