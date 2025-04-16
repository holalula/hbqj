from housingutils import *
import json
class StaticHousingModel:
    def __init__(self, housingname, uploader, housesize: int, tags, items, uploadtime):
        self.houisngname = housingname
        self.uploader = uploader
        self.housesize = housesize
        self.tags = tags
        self.items = items
        self.uploadtime = uploadtime
        self.itemshash = stringtomd5(self.items)

    def ToJsonStr(self) -> str:
        return json.dumps(self, default=lambda o: o.__dict__)

class FurnitureInstance(object):
    def __init__(self,c=0,x=0,y=0,z=0,r=0,color=0):
        self.category= c
        self.color = color
        self.x = x
        self.y = y
        self.z = z
        self.r = r

class CategoryInstance(object):
    def __init__(self,_cid,_count=0,__p=0):
        self.cid = _cid
        self.count = _count
        self.posX = []
        self.posY = []
        self.posZ = []
        self.Rotation = []
        self.Color = []
        self._p = __p

if __name__ == "__main__":
    with open("n.json", 'r') as _file:
        items = _file.read()
    housing = StaticHousingModel("John's housing", "Oliver", 0, ["tag1", "tag2"], items, GetToday())
    print(housing.ToJsonStr())
    with open(housing.itemshash, "w") as f:
        f.write(housing.ToJsonStr())
    