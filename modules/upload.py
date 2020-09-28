import json


def echo(code, html):
    print(json.dumps({
        "responseCode": code,
        "headers": {"Content-Type": ["text/html"]},
        "length": len(html),
        "data": "inline"
    }))
    print()
    print(html)


data = json.loads(input().strip())
success_msg = "<h3>Upload successful</h3>" if data["POST"] else ""
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
