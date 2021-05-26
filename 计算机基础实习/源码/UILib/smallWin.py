# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification.
# No part of this specification may be reproduced in any form or means,
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice.
# Yingping Liang assumes no responsibility for any errors contained herein.


from time import localtime

from PyQt5 import QtWidgets
from PyQt5.uic import loadUi

from UILib.Database import KEYS
from UILib.DetailLogWindow import DetailLogWindow


class ViolationItem (QtWidgets.QListWidget):
    def __init__(self, parent=None):
        super(ViolationItem, self).__init__(parent)
        loadUi("./data/UI/ViolationItem.ui", self)
        self.details_button.clicked.connect(self.showDetails)
        self.data = {}

    def setData(self, data):
        self.data = data
        self.illegal = data[KEYS.RULENAME]
        self.setCarId(data[KEYS.CARID])
        self.setTime(self.get_time())
        self.setCarImage(data[KEYS.CARIMAGE])

    def setIllegal(self):
        if self.illegal == '待定':
            pass

    def get_time(self):

        out = '{}年-{}月-{}日\n{}'
        t = localtime()
        out = out.format(
            t.tm_year, t.tm_mon, t.tm_mday,
            str(t.tm_hour)+':'+str(t.tm_min)+':'+str(t.tm_sec)
        )

        return out

    def showDetails(self):
        window = DetailLogWindow(self.data, self)
        window.show()

    def setCarId(self, id):
        self.carid.setText(str(id))

    def setTime(self, time):
        self.time.setText(time)

    def setCarImage(self, pixmap):
        self.carimage.setPixmap(pixmap)
        self.carimage.show()
