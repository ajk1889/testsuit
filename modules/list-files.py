import json
import mimetypes
import os
import urllib.parse
from html import escape

base_path = os.getcwd()
base_url = "/files"
MAX_NAME_LEN = 30
guess_mime_type = mimetypes.MimeTypes().guess_type

folder_row_template = '''
<tr>
    <td style="text-align: left;"><a href="{folder_link}" style="color:{color}">{folder_name}</a></td>
    <td>{contents_len}</td>
    <td><a href="{download_link}">download</a></td>
</tr>
'''

template = '''
<html>
<head>
    <title>{folder_name}</title>
    <style>
        td {{
            text-align: center;
            padding: 5px 20px;
        }}
        th {{
            text-align: center;
            padding: 5px 20px;
        }}
        a {{
            font-style: normal;
            text-decoration: none;
        }}
    </style>
</head>
<body style="font-family: sans-serif;">
    <a href="/upload?path={folder_path}"><h3>Upload files here</h3></a>
    <h2 style="margin-left: 30px;">Folders</h2>
    <table style="font-size:20px;">
        <thead>
            <tr>
                <th style="text-align: left;">Name</th>
                <th>Contents</th>
                <th></th>
            </tr>
        </thead>
        <tbody>
            {folder_rows}
            {file_rows}
        </tbody>
    </table>
</body>
</html>
'''


def shorten(name):
    if len(name) > MAX_NAME_LEN:
        return name[:int(MAX_NAME_LEN / 2) - 2] + '....' + name[len(name) - int(MAX_NAME_LEN / 2) + 2:]
    else:
        return name


def format_size(size):
    prefixes = ['bytes', 'KB', 'MB', 'GB', 'TB']
    index = 0
    while size > 1000:
        size /= 1024
        index += 1
    if index == 0:
        return f'{size} bytes'
    return '{size} {prefix}'.format(size='%.2f' % size, prefix=prefixes[index])


def get_files_list(folder):
    sub_files = []
    sub_folders = []
    for f in os.listdir(folder):
        if os.path.isdir(os.path.join(folder, f)):
            sub_folders.append(f)
        else:
            sub_files.append(f)
    relative_path = os.path.relpath(folder, base_path)
    relative_path = "" if relative_path == "." else relative_path

    sub_folders.sort()
    folder_rows = []
    for f in sub_folders:
        link = urllib.parse.quote(os.path.join(base_url, relative_path, f))
        download_link = urllib.parse.quote(os.path.join("/download", relative_path, f))
        folder_rows.append(folder_row_template.format(
            folder_link=link,
            contents_len=len(os.listdir(os.path.join(folder, f))),
            folder_name=escape(shorten(f)),
            download_link=download_link,
            color='red'
        ))

    sub_files.sort()
    file_rows = []
    for f in sub_files:
        link = urllib.parse.quote(os.path.join(base_url, relative_path, f))
        download_link = urllib.parse.quote(os.path.join("/download", relative_path, f))
        file_rows.append(folder_row_template.format(
            folder_link=link,
            contents_len=format_size(os.stat(os.path.join(folder, f)).st_size),
            folder_name=escape(shorten(f)),
            download_link=download_link,
            color='darkgreen'
        ))
    return template.format(
        folder_name=os.path.dirname(folder).split('/')[-1],
        file_rows=''.join(file_rows),
        folder_rows=''.join(folder_rows),
        folder_path=folder
    )


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
        echo(200, get_files_list(path))
        exit(0)

    mime_type = guess_mime_type(path)[0]
    if not mime_type:
        mime_type = "application/octet-stream"

    response = {
        "responseCode": 206,
        "headers": {
            "Content-Type": [mime_type],
            "Content-Disposition": [f'attachment; filename="{path.split("/")[-1]}"']
        },
        "data": path
    }
    if mime_type != "application/octet-stream":
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
