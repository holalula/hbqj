import sys
from PyQt5.QtWidgets import (QApplication, QLabel, QMainWindow, QWidget
,QInputDialog)
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from QCandyUi import CandyWindow
from ctypes import *
from hbqj import StartWindow
if __name__ == '__main__':
    app = QApplication(sys.argv)
    t = StartWindow()
    title = t.mainname +t.version
    t = CandyWindow.createWindow(t,'green')
    t.setWindowTitle(title)
    t.resize(450,400)
    t.setWindowIcon(QIcon('hbqj.ico'))
    t.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
    t.show()
    sys.exit(app.exec_())