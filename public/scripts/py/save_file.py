import cgi
import os

try:
    import msvcrt
    msvcrt.setmode(0, os.O_BINARY)  
    msvcrt.setmode(1, os.O_BINARY) 
except ImportError:
    pass

form = cgi.FieldStorage()

fileitem = form['file']
if fileitem.filename:
    upload_directory = os.getenv('UPLOAD_DIRECTORY')

    if not os.path.exists(upload_directory):
        os.makedirs(upload_directory)

    fn = os.path.basename(fileitem.filename)
    target_path = os.path.join(upload_directory, fn)

    with open(target_path, 'wb') as target_file:
        target_file.write(fileitem.file.read())

    message = 'The file <b>{}</b> was uploaded successfully to <b>{}</b>'.format(fn, upload_directory)

else:
    message = 'No file was uploaded'

html_response = '<html><h1>this is python upload test</h1><p> "{}" </p></html>'.format(message)

print(html_response)



