import sys
from PyQt5.QtWidgets import (QApplication, QLabel, QMainWindow, QWidget
,QInputDialog)
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from QCandyUi import CandyWindow
from mover import Mover
from decorator import Decorator
from ctypes import *
import os
import shutil
import datetime
from storage import GetIntroduction
from customQt import ClickQLabel
from config import GetVersion, GetWindowTitle, GetTheme

class StartWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.version = GetVersion()
        self.mainname = GetWindowTitle()
        self.dll = 0
        self.LoadDLL()
        self.InitUI()

    def LoadDLL(self):
        #Copy \\HaiBinQiangJia\\boogiepop.dll
        boogiepopdll = os.path.join(os.getcwd(),"boogiepop.dll")
        appdata = os.environ['APPDATA']
        folder = os.path.join(appdata,'HaiBinQiangJia')
        if not os.path.exists(folder):
            os.makedirs(folder)
        with open(os.path.join(folder,"date"),'w') as f:
            f.write(str(datetime.datetime.now()).split('.')[0].split(' ')[0])
        if os.path.exists(os.path.join(folder,"boogiepop.dll")):
            try:
                os.remove(os.path.join(folder,"boogiepop.dll"))
            except:
                pass
        try:
            shutil.copyfile(boogiepopdll, os.path.join(folder,"boogiepop.dll"))
        except:
            path = os.path.join(os.getcwd(),"HaiBinQiangJia.dll")
            self.dll = windll.LoadLibrary(path)
            self.dll.LoadInjectBoogiepop()
        else:
            path = os.path.join(os.getcwd(),"HaiBinQiangJia.dll")
            self.dll = windll.LoadLibrary(path)
            self.dll.LoadInjectBoogiepop()
#        gameon = self.dll.GameOn()
#        if gameon==0:
#            QMessageBox.information(self,"游戏未启动","请先打开游戏！<br>若游戏已启动请去<a href=\"https://bbs.nga.cn/read.php?tid=22777919\">NGA的这个帖子</a>查看是否需要更新。(https://bbs.nga.cn/read.php?tid=22777919)")
#            sys.exit(0)

    def CreateWindow(self, dll, qwidget, title, size):
        # TODO: multithread (need QThread here, threading not work)
        def create():
            window = CandyWindow.createWindow(qwidget(dll), GetTheme())
            window.setWindowTitle(title)
            window.resize(size[0], size[1])
            window.setWindowIcon(QIcon('hbqj.ico'))
            window.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
            window.show()
        return create

    def InitUI(self):
        self.intro = QLabel()
        self.intro.setOpenExternalLinks(True)
        font = QFont("Microsoft YaHei",14,70)
        self.intro.setFont(font)
        self.intro.setText(GetIntroduction())

        self.decorator = QPushButton("装修工")
        self.decorator.clicked.connect(self.CreateWindow(self.dll, Decorator, '海滨墙架装修工', (300, 500)))
        self.decorator.setMinimumHeight(40)
        self.decorator.setSizePolicy(QSizePolicy.Policy.Expanding,QSizePolicy.Policy.Expanding)

        self.mover = QPushButton("搬家工")
        self.mover.clicked.connect(self.CreateWindow(self.dll, Mover, '海滨墙架搬家工', (1400, 600)))
        self.mover.setMinimumHeight(40)
        self.mover.setSizePolicy(QSizePolicy.Policy.Expanding,QSizePolicy.Policy.Expanding)

        self.updater = QPushButton("检查更新")
        self.updater.clicked.connect(lambda : print("Check Update"))
        self.updater.setMinimumHeight(40)
        self.updater.setSizePolicy(QSizePolicy.Policy.Expanding,QSizePolicy.Policy.Expanding)

        layout = QVBoxLayout()
        layout2 = QVBoxLayout()
        layout2.addWidget(self.intro)
        wgl2 = QWidget()
        wgl2.setLayout(layout2)
        layout.addWidget(wgl2)
        layout.addStretch(1)
        layout.addWidget(self.decorator)
        layout.addStretch(0)
        layout.addWidget(self.mover)
        layout.addStretch(0)
        layout.addWidget(self.updater)
        layout.addStretch(0)
        self.setLayout(layout)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    t = StartWindow()
    title = t.mainname +t.version
    t = CandyWindow.createWindow(t,'blueDeep')
    t.setWindowTitle(title)
    t.resize(450,400)
    t.setWindowIcon(QIcon('hbqj.ico'))
    t.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
    t.show()
    sys.exit(app.exec_())