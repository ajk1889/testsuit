import json
import os
import urllib.parse
from html import escape

data = json.loads(input().strip())
path = data["path"]
if path[-1] == '/': path = path[:-1]
if not os.path.exists(path):
    response = "<h1>File not found</h1>"
    print(json.dumps({
        "responseCode": 404,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(response),
        "data": "inline"
    }))
    print()
    print(response)
elif os.path.isdir(path):
    response = f'''<html>
    <head>
        <title>{path.split('/')[-1]}</title>
    <head>
    <body>'''
    files = os.listdir(path)
    for f in files:
        response += f"<a href='{urllib.parse.quote(path + '/' + f)}'>{escape(f)}</a><br/>"
    response += '</body></html>'
    print(json.dumps({
        "responseCode": 200,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(response),
        "data": "inline"
    }))
    print()
    print(response)
else:
    if "Range" in data["HEADERS"]:
        range = data["HEADERS"]["Range"]
        range = range[range.index('=') + 1:]
        if ',' in range:
            response = "<h3>Multi-range is not supported</h3>"
            print(json.dumps({
                "responseCode": 406,
                "headers": {"Content-Type": ["text/html"]},
                "length": len(response),
                "data": "inline"
            }))
            print()
            print(response)
        else:
            range = range.split('-')
            print(json.dumps({
                "responseCode": 206,
                "headers": {
                    "Content-Type": ["application/octet-stream"],
                    "Content-Disposition": ['attachment; filename="{name}"'.format(name=path.split('/')[-1])]
                },
                "offset": range[0],
                "limit": range[1],
                "data": path
            }))
            print()
    else:
        print(json.dumps({
            "responseCode": 200,
            "headers": {
                "Content-Type": ["application/octet-stream"],
                "Content-Disposition": ['attachment; filename="{name}"'.format(name=path.split('/')[-1])]
            },
            "data": path
        }))
        print()
