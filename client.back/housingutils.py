import hashlib
import uuid
from Crypto.Cipher import AES
from binascii import b2a_base64, a2b_base64
import datetime
import imghdr
import os

def GetHBQJAppPath(filename):
    appdata = os.environ['APPDATA']
    folder = os.path.join(appdata,'HaiBinQiangJia')
    #filename = "__boogiepop_preview.hbqj"
    return os.path.join(folder,filename)

def get_mac_address():
    node = uuid.getnode()
    mac = uuid.UUID(int = node).hex[-12:]
    return mac

def stringtomd5(originstr):
    signaturemd5 = hashlib.md5()
    signaturemd5.update(originstr.encode('utf8'))
    return signaturemd5.hexdigest()

salt = "neversmile"
key = "wjsycdmm"
def rpad(text, divisor: int, suffix):
    remain = len(text) % divisor
    if remain > 0:
        text += suffix * (divisor - remain)
    return text

def encrypt(text, salt = "neversmile", key = "wjsycdmm"):
    fmtkey, fmtiv = map(lambda s: s.encode()[:16].ljust(16, b'\0'), (key, salt))
    cryptor = AES.new(fmtkey, AES.MODE_CBC, fmtiv)
    fmttext = rpad(text.encode(), 16, b'\0')
    ciphertext = cryptor.encrypt(fmttext)
    return str(b2a_base64(ciphertext))[2:-3].rstrip('=')

def decrypt(text, salt = "neversmile", key = "wjsycdmm"):
    fmtkey, fmtiv = map(lambda s: s.encode()[:16].ljust(16, b'\0'), (key, salt))
    cryptor = AES.new(fmtkey, AES.MODE_CBC, fmtiv)
    fmttext = rpad(text, 4, '=')
    return cryptor.decrypt(a2b_base64(fmttext)).rstrip(b'\0').decode()

def GetToday():
    return str(datetime.datetime.now()).split('.')[0]

def GetImageType(file_path):
    img_type = imghdr.what(file_path)
    if img_type is not None:
        return img_type
    return ""
        
if __name__ == "__main__":
    pass