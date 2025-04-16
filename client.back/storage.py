# TODO: cache
import requests
import os
from housingutils import *

import json
base_url = "https://hbqj-storage.pages.dev"
def GetIntroduction():
    return requests.get(base_url + "/intro").text

def GetHousingIndex():
    return requests.get(base_url + "/housings/index.json").json()

def GetHousingByHash(itemshash) -> dict:
    return requests.get(base_url + "/housings/" + itemshash).json()

def GetHousingImageByHash(itemshash):
    return requests.get(base_url + "/housings/images/" + itemshash).content
if __name__ == "__main__":
    #print((GetHousingIndex()))
    #print(GetHousingByHash("74b0ecb3c59bfbf5afeb8919aab51e24"))
    h = "74b0ecb3c59bfbf5afeb8919aab51e24"
    print(json.loads(GetHousingByHash(h)["items"]))
#    with open(h, "wb") as _file:
#        _file.write(GetHousingImageByHash(h))
#    if GetImageType(h) != "":
#        with open(h + "." + GetImageType(h), "wb") as _file:
#            _file.write(GetHousingImageByHash(h))
#            os.remove(h)
    