import json
import os
import urllib.parse
from html import escape

base_path = os.getcwd()


def get_files_list(folder):
    html = f"""<html><head><title>{folder.split('/')[-1]}</title><head><body style="font-size:20px">\n"""
    sub_files = []
    sub_folders = []
    for f in os.listdir(folder):
        if os.path.isdir(os.path.join(folder, f)):
            sub_folders.append(f)
        else:
            sub_files.append(f)
    relative_path = os.path.relpath(folder, base_path)
    relative_path = "" if relative_path == "." else "/" + relative_path

    sub_folders.sort()
    for f in sub_folders:
        html += f'<a href="{urllib.parse.quote(relative_path + "/" + f)}" style="color:red">{escape(f)}</a><br/>\n'
    if sub_folders:
        html += "<br/>\n"

    sub_files.sort()
    for f in sub_files:
        html += f'<a href="{urllib.parse.quote(relative_path + "/" + f)}" style="color:black">{escape(f)}</a><br/>\n'
    html += '</body></html>'
    return html


def echo(code, html):
    print(json.dumps({
        "responseCode": code,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(html),
        "data": "inline"
    }))
    print()
    print(html)


try:
    data = json.loads(input().strip())
    if "base-path" in data["applicationParams"]["additionalKwargs"]:
        base_path = data["applicationParams"]["additionalKwargs"]["base-path"]
    path = data["path"]
    while path and path[0] == '/':
        path = path[1:]
    path = os.path.join(base_path, path)

    if not os.path.exists(path):
        echo(404, "<h1>File not found</h1>")
        exit(0)

    if os.path.isdir(path):
        echo(200, get_files_list(path))
        exit(0)

    mimeType = "application/octet-stream"
    if path.split('.')[-1] == 'html':
        mimeType = 'text/html'

    response = {
        "responseCode": 206,
        "headers": {
            "Content-Type": [mimeType],
            "Content-Disposition": [f'attachment; filename="{path.split("/")[-1]}"']
        },
        "data": path
    }
    if mimeType != "application/octet-stream":
        response["headers"].pop("Content-Disposition")
    if "Range" in data["HEADERS"]:
        contentRange = data["HEADERS"]["Range"][0]
        contentRange = contentRange[contentRange.index('=') + 1:]
        if ',' in contentRange:
            echo(406, "<h3>Multi-range is not supported</h3>")
            exit(0)
        contentRange = contentRange.split('-')
        try:
            response["offset"] = int(contentRange[0])
        except ValueError:
            pass
        try:
            response["limit"] = int(contentRange[1])
        except ValueError:
            pass
        response["responseCode"] = 206
        print(json.dumps(response))
        print()
    else:
        response["responseCode"] = 200
        print(json.dumps(response))
        print()
except Exception as e:
    echo(500, str(e))
