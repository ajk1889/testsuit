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
        response += f"<a href='{urllib.parse.quote(path + '/' + f)}'>{escape(f)}</a><br/>\n"
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
        contentRange = data["HEADERS"]["Range"]
        contentRange = contentRange[contentRange.index('=') + 1:]
        if ',' in contentRange:
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
            contentRange = contentRange.split('-')
            print(json.dumps({
                "responseCode": 206,
                "headers": {
                    "Content-Type": ["application/octet-stream"],
                    "Content-Disposition": [f'attachment; filename="{path.split("/")[-1]}"']
                },
                "offset": contentRange[0],
                "limit": contentRange[1],
                "data": path
            }))
            print()
    else:
        print(json.dumps({
            "responseCode": 200,
            "headers": {
                "Content-Type": ["application/octet-stream"],
                "Content-Disposition": [f'attachment; filename="{path.split("/")[-1]}"']
            },
            "data": path
        }))
        print()
