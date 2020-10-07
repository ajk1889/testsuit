import json
import os
import sys

import zipstream

base_url = "/download"
base_path = os.getcwd()


def list_files(folder, out_list):
    for f in os.listdir(folder):
        if os.path.isfile(os.path.join(folder, f)):
            out_list.append(os.path.join(folder, f))
        else:
            list_files(os.path.join(folder, f), out_list)
    return out_list


def echo(code, html):
    print(json.dumps({
        "responseCode": code,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(html),
        "data": "inline"
    }))
    print()
    print(html)


def send_file(absolute_path):
    mime_type = "application/octet-stream"
    response = {
        "responseCode": 206,
        "headers": {
            "Content-Type": [mime_type],
            "Content-Disposition": [f'attachment; filename="{absolute_path.split("/")[-1]}"']
        },
        "data": absolute_path
    }
    if "Range" in data["HEADERS"]:
        content_range = data["HEADERS"]["Range"][0]
        content_range = content_range[content_range.index('=') + 1:]
        if ',' in content_range:
            echo(406, "<h3>Multi-range is not supported</h3>")
            exit(0)
        content_range = content_range.split('-')
        try:
            response["offset"] = int(content_range[0])
        except ValueError:
            pass
        try:
            response["limit"] = int(content_range[1])
        except ValueError:
            pass
        response["responseCode"] = 206
        print(json.dumps(response))
        print()
    else:
        response["responseCode"] = 200
        print(json.dumps(response))
        print()


def to_zipstream(folder):
    stream = zipstream.ZipFile()
    for file in list_files(folder, []):
        stream.write(file, os.path.relpath(file, folder), zipstream.ZIP_DEFLATED)
    return stream


def send(name, zipstream):
    response = {
        "responseCode": 200,
        "headers": {
            "Content-Type": ["application/zip"],
            "Content-Disposition": [f'attachment; filename="{name}.zip"']
        },
        "data": "inline"
    }
    print(json.dumps(response))
    print()
    sys.stdout.flush()
    sys.stdout.buffer.flush()
    for bfr in zipstream:
        sys.stdout.buffer.write(bfr)
    sys.stdout.buffer.flush()


try:
    data = json.loads(input().strip())
    if "base-path" in data["applicationParams"]["additionalKwargs"]:
        base_path = data["applicationParams"]["additionalKwargs"]["base-path"]
        if base_path[0] == "'" or base_path[0] == '"':
            base_path = base_path[1:]
        if base_path[-1] == "'" or base_path[-1] == '"':
            base_path = base_path[:-1]
    path = data["path"][len(base_url):]
    while path and path[0] == '/':
        path = path[1:]
    path = os.path.join(base_path, path)
    if not os.path.exists(path):
        echo(404, "<h1>File not found</h1>")
        exit(0)
    if os.path.isdir(path):
        send(path.split('/')[-1], to_zipstream(path))
    else:
        send_file(path)
except Exception as e:
    echo(500, str(e))
