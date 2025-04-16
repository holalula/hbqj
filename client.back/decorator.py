import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt, QSortFilterProxyModel
from PyQt5.QtWidgets import QCompleter, QComboBox, QSlider
from QCandyUi import CandyWindow
from ctypes import *
from config import *
from storage import *

class Decorator(QWidget):
    def __init__(self,dll):
        super(Decorator, self).__init__()

        self.dll = dll
        #if self.dll.GameOn()==1:
        #    self.shadow()
        self.InitUI()
        
        self.timer_UpdatePos1 = QTimer()
        self.timer_UpdatePos2 = QTimer()
        self.timer_UpdatePos1.timeout.connect(self.QTimer_UpdatePos1)
        self.timer_UpdatePos2.timeout.connect(self.QTimer_UpdatePos2)

    def Pos2CanEdit(self):
        return self.dll.CanEdit()

    def QTimer_UpdatePos1(self):
        pos = self.t1_getpos()
        self.t1labelposx.setText('%.3f'%pos[0])
        self.t1labelposy.setText('%.3f'%pos[1])
        self.t1labelposz.setText('%.3f'%pos[2])

    def QTimer_UpdatePos2(self):
        if(self.Pos2CanEdit()==1):
            pos = self.t2_getpos()
            self.t2_le_posx.setText('%.3f'%pos[0])
            self.t2_le_posy.setText('%.3f'%pos[1])
            self.t2_le_posz.setText('%.3f'%pos[2])

    def anywhere(self):
        if(self.placeanywhere.isChecked()):
            self.PlaceAnywhere()
        else:
            self.OffPlaceAnywhere()

    def imguizmo(self):
        if(self.guizmo.isChecked()):
            self.OnGuizmo()
        else:
            self.OffGuizmo()

    def wheelEvent(self, event: QtGui.QWheelEvent) -> None:
        if(self.t2_cb.isChecked()):
            sig = 1 if event.angleDelta().y()>0 else -1
            if(self.t2_le_posx.hasFocus()):
                newposx = ((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posx.text()))
                self.t2_setX(float(newposx))
                self.t2_le_posx.setText('%.3f'%((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posx.text())))
            elif(self.t2_le_posy.hasFocus()):
                newposy = ((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posy.text()))
                self.t2_setY(float(newposy))
                self.t2_le_posy.setText('%.3f'%((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posy.text())))
            elif(self.t2_le_posz.hasFocus()):
                newposz = ((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posz.text()))
                self.t2_setZ(newposz)
                self.t2_le_posz.setText('%.3f'%((sig)*float(self.t2_le_delta.text())+float(self.t2_le_posz.text())))

        return super().wheelEvent(event)

    def PlaceAnywhere(self):
        self.dll.OnPlaceAnywhere()
    def OffPlaceAnywhere(self):
        self.dll.OffPlaceAnywhere()
    def t1_getpos(self):
        #DLLEXPORT float* GetPos1(float *pos)
        POS_INPUT = c_float*3
        data = POS_INPUT()
        self.dll.GetPos1.restype = POINTER(c_float)
        res = self.dll.GetPos1(data)
        for i in range(3):
            print(res[i])  
        return res      
    def t1_setX(self,x):
        self.dll.SetPosX1(c_float(x))
    def t1_setY(self,y):
        self.dll.SetPosY1(c_float(y))
    def t1_setZ(self,z):
        self.dll.SetPosZ1(c_float(z))
    def t2_setX(self,x):
        self.dll.SetPosX2(c_float(x))
    def t2_setY(self,y):
        self.dll.SetPosY2(c_float(y))
    def t2_setZ(self,z):
        self.dll.SetPosZ2(c_float(z))            
    def t2_getpos(self):
        POS_INPUT = c_float*3
        data = POS_INPUT()
        self.dll.GetPos2.restype = POINTER(c_float)
        res = self.dll.GetPos2(data)
        return res  

    #Guizmo
    def OnGuizmo(self):
        self.dll.OnGuizmo()
    def OffGuizmo(self):
        self.dll.OffGuizmo()
    def OnGuizmoSnap(self):
        self.dll.OnGuizmoSnap()
    def OffGuizmoSnap(self):
        self.dll.OffGuizmoSnap()
    def SetGuizmoSnap(self,_s):
        self.dll.SetGuizmoSnap(c_float(float(_s)))

    def cb_post1(self):
        if self.t1_cb.isChecked():
            self.timer_UpdatePos1.start(100)
        else:
            self.timer_UpdatePos1.stop()

    def cb_post2(self):
        if self.t2_cb.isChecked():
            self.timer_UpdatePos2.start(100)
        else:
            self.timer_UpdatePos2.stop()

    def SavePreview(self):
        if self.persis_preview.isChecked():
            reply = QMessageBox.question(self,"Are you sure?","still testing",QMessageBox.Yes|QMessageBox.No,QMessageBox.No)
            if reply == QMessageBox.No:
                return
            self.dll.OnSavePreview()
        else:
            self.dll.OffSavePreview()

    def InitUI(self):
        self.intro = QLabel()
        self.intro.setOpenExternalLinks(True)
        font = QFont("Microsoft YaHei",10,63)
        self.intro.setFont(font)
        self.intro.setText(GetIntroduction())

        layout = QVBoxLayout()
        layout.addWidget(self.intro)
        self.placeanywhere = QCheckBox("解除摆放限制")
        self.placeanywhere.stateChanged.connect(self.anywhere)
        #self.placeanywhere.stateChanged.connect(self.cb_stateChange)
        self.guizmo = QCheckBox("游戏内启用坐标轴移动")
        self.guizmo.stateChanged.connect(self.imguizmo)
        self.le_guizmosnap = QLineEdit("0.0")
        self.persis_preview = QCheckBox("保存预览的家具(测试)")

        self.change_time = QCheckBox("改变房间内时间")
        self.time_value = QSlider(Qt.Horizontal)
        self.time_max_value = QLineEdit("100000 (default max time)")
        self.time_max_value.editingFinished.connect(lambda :self.time_value.setMaximum(int(self.time_max_value.text())))
        self.time_value.setMinimum(0)
        self.time_value.setMaximum(100000)
        def SetTimeValue():
            if self.change_time.isChecked():
                print(self.time_value.value())
                self.dll.SetTime(c_int(self.time_value.value()))
        def DisableSetTimeValue():
            if not self.change_time.isChecked():
                self.dll.DisableSetTime()
        self.time_value.valueChanged.connect(SetTimeValue)
        self.change_time.stateChanged.connect(DisableSetTimeValue)


        self.tabs = QTabWidget()
        self.t1 = QWidget()
        self.t2 = QWidget()
        self.tabs.addTab(self.t1,"一")
        self.tabs.addTab(self.t2,"二")

        self.t1posx = QLineEdit("0.0")
        self.t1posy = QLineEdit("0.0")
        self.t1posz = QLineEdit("0.0")
        self.t1labelposx = QLabel('0.0')
        self.t1labelposy = QLabel('0.0')
        self.t1labelposz = QLabel('0.0')

        qfl = QFormLayout()
        qfl.addRow(self.t1labelposx,self.t1posx)
        qfl.addRow(self.t1labelposy,self.t1posy)
        qfl.addRow(self.t1labelposz,self.t1posz)

        self.t1_cb = QCheckBox('修改坐标一')
        self.t1_copy = QPushButton("===>复制===>")
        self.t1_copy.setMinimumHeight(50)

        self.t1_layout = QVBoxLayout()
        self.t1_layout.addWidget(self.placeanywhere)
        self.t1_layout.addWidget(self.guizmo)
        self.t1_layout.addWidget(self.le_guizmosnap)
        self.t1_layout.addWidget(self.change_time)
        self.t1_layout.addWidget(self.time_value)
        self.t1_layout.addWidget(self.time_max_value)
        #self.t1_layout.addWidget(self.persis_preview)
        self.t1_widget = QWidget()
        self.t1_widget.setLayout(qfl)
        self.t1_layout.addWidget(self.t1_cb)
        self.t1_layout.addWidget(self.t1_copy)
        self.t1_layout.addWidget(self.t1_widget)
        self.t1.setLayout(self.t1_layout)

        self.t1posx.setValidator(QDoubleValidator(float("-inf"),float("inf"),3))
        self.t1posy.setValidator(QDoubleValidator(float("-inf"),float("inf"),3))
        self.t1posz.setValidator(QDoubleValidator(float("-inf"),float("inf"),3))
        self.t1posx.editingFinished.connect(lambda :{self.t1_setX(float(self.t1posx.text()))})
        self.t1posy.editingFinished.connect(lambda :{self.t1_setY(float(self.t1posy.text()))})
        self.t1posz.editingFinished.connect(lambda :{self.t1_setZ(float(self.t1posz.text()))})
        self.t1_copy.clicked.connect(lambda :{self.t1posx.setText(self.t1labelposx.text()),self.t1posy.setText(self.t1labelposy.text()),self.t1posz.setText(self.t1labelposz.text())})

        self.t2_cb = QCheckBox("修改坐标二(旋转状态)")
        self.t2_le_delta = QLineEdit("0.05")
        self.t2_le_posx = QLineEdit("0.0")
        self.t2_le_posy = QLineEdit("0.0")
        self.t2_le_posz = QLineEdit("0.0")
        self.t2_pb_subx = QPushButton("-X")
        self.t2_pb_addx = QPushButton("+X")
        self.t2_pb_suby = QPushButton("-Y")
        self.t2_pb_addy = QPushButton("+Y")
        self.t2_pb_subz = QPushButton("-Z")
        self.t2_pb_addz = QPushButton("+Z")

        self.t2_layout = QVBoxLayout()
        self.t2_layout.addWidget(self.t2_cb)
        self.t2_layout.addWidget(self.t2_le_delta)
        self.t2_layout.addWidget(self.t2_le_posx)
        wx = QHBoxLayout()
        wx.addWidget(self.t2_pb_subx)
        wx.addWidget(self.t2_pb_addx)
        wx1 = QWidget()
        wx1.setLayout(wx)
        self.t2_layout.addWidget(wx1)
        wy = QHBoxLayout()
        wy.addWidget(self.t2_pb_suby)
        wy.addWidget(self.t2_pb_addy)
        wy1 = QWidget()
        wy1.setLayout(wy)
        self.t2_layout.addWidget(self.t2_le_posy)
        self.t2_layout.addWidget(wy1)
        wz = QHBoxLayout()
        wz.addWidget(self.t2_pb_subz)
        wz.addWidget(self.t2_pb_addz)
        wz1 = QWidget()
        wz1.setLayout(wz)
        self.t2_layout.addWidget(self.t2_le_posz)
        self.t2_layout.addWidget(wz1)
        self.t2.setLayout(self.t2_layout)
        layout.addWidget(self.tabs)
        self.setLayout(layout)

        self.t2_le_posx.setValidator(QDoubleValidator(float("-inf"),float("inf"),6))
        self.t2_le_posy.setValidator(QDoubleValidator(float("-inf"),float("inf"),3))
        self.t2_le_posz.setValidator(QDoubleValidator(float("-inf"),float("inf"),3))
        self.t2_pb_subx.clicked.connect(lambda :{self.t2_setX(float((-1*float(self.t2_le_delta.text())+float(self.t2_le_posx.text())))),self.t2_le_posx.setText('%.3f'%(-1*float(self.t2_le_delta.text())+float(self.t2_le_posx.text())))})
        self.t2_pb_addx.clicked.connect(lambda :{self.t2_setX(float((1*float(self.t2_le_delta.text())+float(self.t2_le_posx.text())))),self.t2_le_posx.setText('%.3f'%(1*float(self.t2_le_delta.text())+float(self.t2_le_posx.text())))})
        self.t2_pb_suby.clicked.connect(lambda :{self.t2_setY(float((-1*float(self.t2_le_delta.text())+float(self.t2_le_posy.text())))),self.t2_le_posy.setText('%.3f'%(-1*float(self.t2_le_delta.text())+float(self.t2_le_posy.text())))})
        self.t2_pb_addy.clicked.connect(lambda :{self.t2_setY(float((1*float(self.t2_le_delta.text())+float(self.t2_le_posy.text())))),self.t2_le_posy.setText('%.3f'%(1*float(self.t2_le_delta.text())+float(self.t2_le_posy.text())))})
        self.t2_pb_subz.clicked.connect(lambda :{self.t2_setZ(float((-1*float(self.t2_le_delta.text())+float(self.t2_le_posz.text())))),self.t2_le_posz.setText('%.3f'%(-1*float(self.t2_le_delta.text())+float(self.t2_le_posz.text())))})
        self.t2_pb_addz.clicked.connect(lambda :{self.t2_setZ(float((1*float(self.t2_le_delta.text())+float(self.t2_le_posz.text())))),self.t2_le_posz.setText('%.3f'%(1*float(self.t2_le_delta.text())+float(self.t2_le_posz.text())))})

        self.t1_cb.stateChanged.connect(self.cb_post1)
        self.t2_cb.stateChanged.connect(self.cb_post2)

        self.persis_preview.stateChanged.connect(self.SavePreview)

        self.le_guizmosnap.editingFinished.connect(self.GuizmoSnap)
    def GuizmoSnap(self):
        #print('%.3f'%(float(self.le_guizmosnap.text())))
        snap = float(self.le_guizmosnap.text())
        if snap==0:
            self.dll.OffGuizmoSnap()
        else:
            self.dll.SetGuizmoSnap(c_float(snap))
            self.dll.OnGuizmoSnap()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main = Decorator(0)
    main = CandyWindow.createWindow(main, GetTheme())
    main.setWindowTitle('holalula')
    main.resize(300,500)
    main.show()
    sys.exit(app.exec_())