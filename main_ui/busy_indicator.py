from PySide6.QtCore import QObject, Qt, Slot
from PySide6.QtWidgets import QWidget, QProgressBar


class BusyIndicator(QProgressBar):
    def __init__(self):
        super().__init__()
        self.setMaximum(0)
        self.setTextVisible(False)
        self._count = 0
        self.hide()

    @Slot(None)
    def show(self):
        super().show()
        self._count += 1

    @Slot(None)
    def hide(self):
        if self._count > 0:
            self._count -= 1

        if self._count == 0:
            super().hide()
