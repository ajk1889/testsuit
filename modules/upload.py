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
    if os.path.exists(file_path) and os.path.isdir(file_path):
        return os.path.join(file_path, relative_path)
    elif file_path[-1] == '/':
        os.makedirs(file_path)
        return os.path.join(file_path, relative_path)
    else:
        return file_path


def save_file(file):
    if file:
        file_name = find_file_name(file["headers"])
        if file_name == "":
            return False
        else:
            file_data = file["data"]["data"]
            if data == "":
                return False
            if file["data"]["isFile"]:
                shutil.move(file_data, get_output_path(file_name))
            else:
                with open(get_output_path(file_name), 'wb') as op:
                    op.write(file_data)
            return True
    else:
        return False


data = json.loads(input().strip())
post = data["POST"]
if post:
    success_msg = "<h3>Upload successful</h3>"
    if "path" in post and post["path"]:
        file_path = post["path"][0]["data"]["data"]
        if file_path[0] == '~':
            file_path = os.path.expanduser(file_path)
    else:
        file_path = os.getcwd()
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
