import json
import os
import random
import shutil


def echo(code, html):
    print(json.dumps({
        "responseCode": code,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(html),
        "data": "inline"
    }))
    print()
    print(html)


def find_file_name(headers):
    disposition = headers["Content-Disposition"][0]
    if "filename=" in disposition:
        name = disposition.split("filename=")[1]
        if name[0] == '"' or name[0] == "'":
            name = name[1:]
        if name[-1] == '"' or name[-1] == "'":
            name = name[:-1]
        return name
    else:
        return str(random.randint(10 ** 9, 10 ** 10))


def get_output_path(relative_path):
    if os.path.exists(folder_path) and os.path.isdir(folder_path):
        return os.path.join(folder_path, relative_path)
    elif folder_path[-1] == '/':
        os.makedirs(folder_path)
        return os.path.join(folder_path, relative_path)
    else:
        return folder_path


def save_file(file):
    if not file: return False
    file_name = find_file_name(file["headers"])
    if not file_name: return False
    file_data = file["data"]["data"]
    if not file_data: return False

    output_path = get_output_path(file_name)
    parent_folder = os.path.dirname(output_path)
    if not os.path.exists(parent_folder):
        os.makedirs(parent_folder)

    if file["data"]["isFile"]:
        shutil.move(file_data, output_path)
    else:
        with open(output_path, 'wb') as op:
            op.write(file_data.encode('utf-8'))
    return True


data = json.loads(input().strip())
post = data["POST"]
if post:
    success_msg = "<h3>Upload successful</h3>"
    if "path" in post and post["path"]:
        folder_path = post["path"][0]["data"]["data"]
        if folder_path[0] == '~':
            folder_path = os.path.expanduser(folder_path)
    else:
        folder_path = os.getcwd()
    if "folder" in post:
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)
        for file_obj in post["folder"]:
            save_file(file_obj)
    if "file" in post:
        save_file(post["file"][0])
else:
    success_msg = ""
response = f'''
<html>
    <title>Upload files</title>
    <body>
        {success_msg}
        <form method="POST" enctype="multipart/form-data" action="/upload">
            Uploaded file store path: 
            <input type="text" name="path" value="/home/"><br/><br/>
            File:
            <input type="file" name="file"><br/><br/>
            Folder:
            <input type="file" webkitdirectory mozdirectory name="folder"/><br/>
            <br/><br/>
            <input type="submit" value="upload folder">
        </form>
    </body>
</html>
'''

echo(200, response)
