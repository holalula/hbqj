from PyQt5.QtWidgets import QCompleter, QComboBox
from PyQt5.QtWidgets import QLabel
from PyQt5.QtCore import Qt, QSortFilterProxyModel
from PyQt5.QtCore import pyqtSignal
'''
    A label that can be clicked.
    Usage:
        thxfont = QFont("Microsoft YaHei",14,70)
        thxfont.setUnderline(True)
        self.thx = ClickQLabel("Sample Label")
        self.thx.setFont(thxfont)
        self.thx.setStyleSheet("color:rgb({},{},{},255)".format(0,154,214))
        self.thx.connect_customized_slot(self.Thx)
'''
class ClickQLabel(QLabel):
    # Custom signal, note that the signal must be a class attribute
    button_clicked_signal = pyqtSignal()
    def __init__(self, parent=None):
        super(ClickQLabel, self).__init__(parent)
    def mouseReleaseEvent(self, QMouseEvent):
        self.button_clicked_signal.emit()
    # Can be linked externally with slots
    def connect_customized_slot(self, func):
        self.button_clicked_signal.connect(func)

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