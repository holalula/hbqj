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
import subprocess
from config import *
from storage import *

class ExtendedComboBox(QComboBox):
    def __init__(self, parent=None):
        super(ExtendedComboBox, self).__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)
        self.setEditable(True)

        # add a filter model to filter matching items
        self.pFilterModel = QSortFilterProxyModel(self)
        self.pFilterModel.setFilterCaseSensitivity(Qt.CaseInsensitive)
        self.pFilterModel.setSourceModel(self.model())

        # add a completer, which uses the filter model
        self.completer = QCompleter(self.pFilterModel, self)
        # always show all (filtered) completions
        self.completer.setCompletionMode(QCompleter.UnfilteredPopupCompletion)
        self.setCompleter(self.completer)

        # connect signals
        self.lineEdit().textEdited.connect(self.pFilterModel.setFilterFixedString)
        self.completer.activated.connect(self.on_completer_activated)


    # on selection of an item from the completer, select the corresponding item from combobox
    def on_completer_activated(self, text):
        if text:
            index = self.findText(text)
            self.setCurrentIndex(index)
            self.activated[str].emit(self.itemText(index))


    # on model change, update the models of the filter and completer as well
    def setModel(self, model):
        super(ExtendedComboBox, self).setModel(model)
        self.pFilterModel.setSourceModel(model)
        self.completer.setModel(self.pFilterModel)


    # on model column change, update the model column of the filter and completer as well
    def setModelColumn(self, column):
        self.completer.setCompletionColumn(column)
        self.pFilterModel.setFilterKeyColumn(column)
        super(ExtendedComboBox, self).setModelColumn(column)

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

class HousingCloud(QWidget):
    def __init__(self,parent):
        super(HousingCloud, self).__init__()
        self.tableWidget = QTableWidget()
        self.InitUI()
        self.SetTable()

        #self.LoadHousingDB()
        self.page_housings = []
        self.LoadStaticHousings()
        self.parent = parent

    def LoadStaticHousings(self):
        #['名称', '上传者', '房屋大小','标签','预览图','上传时间'])
        for housing in GetHousingIndex():
            self.page_housings.append(housing)
            self.tableWidget.insertRow(self.tableWidget.rowCount())
            r = self.tableWidget.rowCount()-1
            self.tableWidget.setItem(r,0,QTableWidgetItem(str(housing["housingname"]).strip()))
            self.tableWidget.setItem(r,1,QTableWidgetItem(str(housing["uploader"]).strip()))
            housesizename ={0:"S",1:"M",2:"L", 3:"公寓"}
            self.tableWidget.setItem(r,2,QTableWidgetItem(str(housesizename[housing["housesize"]])))
            self.tableWidget.setItem(r,3,QTableWidgetItem(str(housing["tags"])))
            btn_img = QPushButton("显示图片")
            btn_img.clicked.connect(self.LoadPreviewImage)
            self.tableWidget.setCellWidget(r,4,btn_img)
            self.tableWidget.setItem(r,5,QTableWidgetItem(str(housing["uploadtime"])))
    def InitUI(self):
        layout = QVBoxLayout()
        self.cb_s = QCheckBox("S")
        self.cb_m = QCheckBox("M")
        self.cb_l = QCheckBox("L")
        self.le_search = QLineEdit("")
        self.btn_upload = QPushButton("上传装修")
        self.btn_refresh = QPushButton("重新加载列表")

        font = QFont("Microsoft YaHei",10,63)

        self.status = QtWidgets.QLabel()
        self.status.setText("Dynamic Text about Status")
        self.status.setObjectName("intro")
        self.status.setFont(font)
        # self.verticalLayout.addWidget(self.status)

        layout_2 = QHBoxLayout()
        # layout_2.addWidget(self.cb_s)
        # layout_2.addWidget(self.cb_m)
        # layout_2.addWidget(self.cb_l)
        # layout_2.addWidget(self.le_search)
        # layout_2.addWidget(self.btn_upload)
        # layout_2.addWidget(self.btn_refresh)
        layout_2.addWidget(self.status)
        self.wg = QWidget()
        self.wg.setLayout(layout_2)
        # TODO: add filter
        layout.addWidget(self.wg)
        layout.addWidget(self.tableWidget)
        self.setLayout(layout)

        self.btn_upload.clicked.connect(self.UploadHousing)

    def UploadHousing(self):
        filepath, filetype = QFileDialog.getOpenFileName(self,"选择一个装修文件","","HBQJ Files (*.hbqj);;Json Files (*.json)")
        print(filepath)

    def SetTable(self):
        self.tableWidget.setColumnCount(6)
        self.tableWidget.setHorizontalHeaderLabels(['名称', '上传者', '房屋大小','标签', '预览图','上传时间'])
        self.tableWidget.setColumnWidth(0, 150)
        self.tableWidget.setColumnWidth(1, 150)
        self.tableWidget.setColumnWidth(2, 40)
        self.tableWidget.setColumnWidth(3, 230)
        self.tableWidget.setColumnWidth(4, 90)
        self.tableWidget.setColumnWidth(5, 150)
        self.tableWidget.setContextMenuPolicy(Qt.CustomContextMenu)  #允许右键产生子菜单
        self.tableWidget.customContextMenuRequested.connect(self.generateMenu)  #右键菜单
    def generateMenu(self, pos):
        #print(pos)
        row_num = -1
        for i in self.tableWidget.selectionModel().selection().indexes():
            row_num = i.row()
        #print(row_num)
        if True:
            menu = QMenu()
            item1 = menu.addAction("导入这个装修")
            #if not sip.isdeleted(self.tableWidget):
            gpos = self.tableWidget.mapToGlobal(pos)
            action = menu.exec_(QPoint(gpos.x(),gpos.y()+40))
            #action = menu.exec_(pos)
            if action == item1:
                self.LoadHousingParent(row_num)
            else:
                return
    
    def LoadHousingParent(self,nrow):
        housing_dict = json.loads(GetHousingByHash(self.page_housings[nrow]["itemshash"])["items"])
        self.parent.LoadJsonDict(housing_dict, cloud = True)

    def ClearTable(self):
        self.tableWidget.clearContents()
        self.tableWidget.setRowCount(0)
    def GetHBQJAppPath(self,filename):
        appdata = os.environ['APPDATA']
        folder = os.path.join(appdata,'HaiBinQiangJia')
        #filename = "__boogiepop_preview.hbqj"
        return os.path.join(folder,filename)

    def LoadPreviewImage(self):
        sender = self.sender()
        if sender:
            row = self.tableWidget.indexAt(sender.pos()).row() 
            itemshash = self.page_housings[row]["itemshash"]
            housingname = self.page_housings[row]["housingname"]
            imgpath = self.GetHBQJAppPath(housingname+"_"+itemshash+"_preview.jpg")
            if os.path.exists(imgpath):
                subprocess.Popen(['mspaint.exe', imgpath])
                return
            image_bytes = GetHousingImageByHash(itemshash)
            image_itemshash_path = self.GetHBQJAppPath(itemshash) 
            with open(image_itemshash_path, 'wb') as _file:
                _file.write(image_bytes)
            image_type = GetImageType(image_itemshash_path)
            if image_type != "":
                image_path = self.GetHBQJAppPath(itemshash + "_" + housingname + "." + image_type)
                with open(image_path, 'wb') as _file:
                    os.remove(image_itemshash_path)
                    _file.write(image_bytes)
                    subprocess.Popen(['mspaint.exe', image_path])
            else:
                QMessageBox.information(self, "Error", "这个装修似乎没有图片预览")
            

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main = HousingCloud(0)
    main = CandyWindow.createWindow(main,'blue')
    main.resize(900,600)
    main.setWindowTitle('holalula')
    main.show()
    sys.exit(app.exec_())