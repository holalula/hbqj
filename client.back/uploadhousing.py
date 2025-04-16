import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt, QSortFilterProxyModel
from PyQt5.QtWidgets import QCompleter, QComboBox
from QCandyUi import CandyWindow
import json
import os
import json
from housingutils import *
from housingmodel import *
from zipfile import ZipFile
class UploadHousing(QWidget):
    def __init__(self):
        super(UploadHousing,self).__init__()
        self.InitUI()
        self.imagepath = ""
        self.itemspath = ""
        self.filetype = ""
        self.itemsjsondict = {}
        self.btn_image.clicked.connect(self.Btn_image)
        self.btn_items.clicked.connect(self.Btn_items)
        self.btn_upload.clicked.connect(self.Btn_upload)

    def InitUI(self):
        layout = QFormLayout()

        self.label_housingname = QLabel("装修名：")
        self.label_uploader = QLabel("上传者：")
        self.label_housingsize = QLabel("房屋大小：")
        self.label_tags = QLabel("描述tags：")
        self.label_image = QLabel("预览图")
        self.label_items = QLabel("装修文件：")

        self.le_housingname = QLineEdit("")
        self.le_uploader = QLineEdit("")
        self.box_housingsize = QComboBox()
        self.box_housingsize.addItems(["S","M","L", "公寓"])
        self.le_tags = QLineEdit("#tag1#tag2")
        self.btn_image = QPushButton("上传预览图")
        self.btn_items = QPushButton("上传装修文件")

        layout.addRow(self.label_housingname,self.le_housingname)
        layout.addRow(self.label_uploader,self.le_uploader)
        layout.addRow(self.label_housingsize,self.box_housingsize)
        layout.addRow(self.label_tags,self.le_tags)
        layout.addRow(self.label_image,self.btn_image)
        layout.addRow(self.label_items,self.btn_items)

        self.btn_upload = QPushButton("打包上传🥰")
        self.btn_upload.setSizePolicy(QSizePolicy.Policy.Expanding,QSizePolicy.Policy.Expanding)
        wg = QWidget()
        wg.setLayout(layout)

        layout2 = QVBoxLayout()
        layout2.addWidget(wg)
        layout2.addWidget(self.btn_upload)

        self.setLayout(layout2)
    
    def Btn_image(self):
        filepath, filetype = QFileDialog.getOpenFileName(self,"选择装修预览图","","Image(*.jpg;*.png)")
        if(os.path.getsize(filepath)>5*1024*1024):
            QMessageBox.information(self,"图片太大了😂！","确保预览图小于5MB哦")
            return
        self.imagepath = filepath
        
    def checkJson(self,filepath):
        with open(filepath,'r') as f:
            res = json.load(f)
        ks = list(res.keys())
        if 'size' in ks and 'list' in ks:
            return True
        else:
            return False

    def checkJsonDict(self,jsondict):
        res = jsondict
        ks = list(res.keys())
        if 'size' in ks and 'list' in ks:
            return True
        else:
            return False

    def Btn_items(self):
        filepath, filetype = QFileDialog.getOpenFileName(self,"选择装修文件","","装修数据(*.hbqj);;装修数据(*.json))")
        if(os.path.getsize(filepath)>5*1024*1024):
            QMessageBox.information(self,"太大了😅！","你的文件太大了！")
            return
        if 'hbqj' in filetype:
            with open(filepath,'r') as f:
                encrystr = f.read()
            destr = decrypt(encrystr,salt,key)
            jsondict = json.loads(destr)
            if(not self.checkJsonDict(jsondict)):
                QMessageBox.information(self,"???","格式错误。")
                return
            self.filetype = "hbqj"
            self.itemsjsondict = jsondict
            self.itemspath = "*.hbqj"
        else: #.json
            if(not self.checkJson(filepath)):
                QMessageBox.information(self,"???","格式错误。")
                return
            self.itemspath = filepath
            self.filetype = "json"

    def Btn_upload(self):
        UPLOAD_FOLDER = 'upload'
        IMAGES_FOLDER = os.path.join(UPLOAD_FOLDER,'images')
        if not os.path.exists(UPLOAD_FOLDER):
            os.mkdir(UPLOAD_FOLDER)
        if not os.path.exists(IMAGES_FOLDER):
            os.mkdir(IMAGES_FOLDER)

        reply = QMessageBox.question(self,"没点错吧?","你确认要打包吗？",QMessageBox.Yes|QMessageBox.No,QMessageBox.No)
        if reply == QMessageBox.No:
            return

        housingname = self.le_housingname.text()
        uploader = self.le_uploader.text()
        housesize = self.box_housingsize.currentIndex()
        tags = self.le_tags.text()
        uploadtime = GetToday()

        #images = img
        #items = json.dumps(res)
        #itemshash = stringtomd5(items)
        img = bytes()
        if self.imagepath!="":
            with open(self.imagepath,'rb') as fp:
                img = fp.read()
        # TODO upload img
        if self.itemspath=="":
            QMessageBox.warning(self,"???","至少要上传装修文件")
            return
        else:
            if self.filetype == "json":
                with open(self.itemspath,'r') as f:
                    res = json.load(f)
                items = json.dumps(res)
            elif self.filetype == "hbqj":
                items = json.dumps(self.itemsjsondict)
            else:
                QMessageBox.warning(self,"???","文件格式错误！")
                return

        housing = StaticHousingModel(housingname, uploader, housesize, [tags], items, GetToday())
        housing_file_name = housing.houisngname + "_" + housing.itemshash
        houisng_file_path = os.path.join(UPLOAD_FOLDER, housing_file_name)
        with open(houisng_file_path, "w") as f:
            f.write(housing.ToJsonStr())
        housingimage_file_name = housing_file_name
        housingimage_file_path = os.path.join(IMAGES_FOLDER, housingimage_file_name)
        with open(housingimage_file_path, 'wb') as f:
            f.write(img)
        
        # write zip file
        zip_file_name = housing_file_name + ".zip"
        zip_file_path = os.path.join(UPLOAD_FOLDER, zip_file_name)
        with ZipFile(zip_file_path, 'w') as zip_obj:
            zip_obj.write(houisng_file_path)
            zip_obj.write(housingimage_file_path)

        explanation = '''
        请将{}目录下的{}文件发送到邮箱：hbqj_upload@163.com. 非常感谢😉！
        '''.format(UPLOAD_FOLDER, zip_file_name)
        explanation_file_path = os.path.join(UPLOAD_FOLDER, "说明文件.txt")
        with open(explanation_file_path, "w") as f:
            f.write(explanation)
            os.startfile(UPLOAD_FOLDER)
            os.startfile(explanation_file_path)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main = UploadHousing()
    main = CandyWindow.createWindow(main,'green')
    main.resize(400,300)
    main.setWindowTitle('holalula')
    main.show()
    sys.exit(app.exec_())