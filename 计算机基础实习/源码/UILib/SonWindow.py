# Copyright © 2020, Yingping Liang. All Rights Reserved.

# Copyright Notice
# Yingping Liang copyrights this specification.
# No part of this specification may be reproduced in any form or means,
# without the prior written consent of Yingping Liang.


# Disclaimer
# This specification is preliminary and is subject to change at any time without notice.
# Yingping Liang assumes no responsibility for any errors contained herein.


import os
import cv2
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtWidgets import QMainWindow, QStatusBar, QListWidget, QAction, qApp, QMenu, QVBoxLayout
from PyQt5.uic import loadUi
import pyqtgraph as pg
from PyQt5.QtWidgets import QSizePolicy
from processor.MainProcessor import MainProcessor
from func.ip_utils import click_get_ip
from numpy import copy
import numpy as np
from imutils import resize
from UILib.smallWin import ViolationItem
from UILib.Database import KEYS


class MainWindow(QMainWindow):
    def __init__(self):

        super(MainWindow, self).__init__()
        loadUi("./data/UI/MainWindowSon.ui", self)

        self.show_frame = cv2.imread('data/show.jpg')
        self.cam_clear_gaurd = False
        self.myip = None
        self.face_id = None

        self.log_tabwidget.clear()
        self.cars_list = QListWidget(self)
        self.illegal_list = QListWidget(self)
        self.log_tabwidget.addTab(self.cars_list, "患者记录")
        self.log_tabwidget.addTab(self.illegal_list, "危险记录")

        self.initMenu()
        self.initParams()
        self.connect_net.clicked.connect(self.start_tcp_server)

        self.feed = None
        self.vs = None
        self.video = None

        self.graph_values = []
        self.eye_values = []
        self.updateCamInfo(None)
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_image)
        self.timer.start(50)

    def initParams(self):
        self.cam_id.setReadOnly(True)
        ret, self.myip = click_get_ip()
        if ret:
            self.cam_id.setText(str(self.myip))
        else:
            self.cam_id.setText('异常，无法获取本机IP')

    def close_all(self):
        if not self.vs is None:
            self.vs.release()

    def initMenu(self):

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&文件')

        addRec = QMenu("添加记录", self)

        act = QAction('添加救护车', self)
        act.setStatusTip('Add Camera Manually')
        addRec.addAction(act)

        act = QAction('添加患者记录', self)
        act.setStatusTip('Add Car Manually')
        addRec.addAction(act)

        fileMenu.addMenu(addRec)

        # Add Exit
        fileMenu.addSeparator()
        act = QAction('&退出', self)
        act.setStatusTip('退出应用')
        act.triggered.connect(self.close_all)
        act.triggered.connect(self.tcp_close)
        act.triggered.connect(qApp.quit)
        fileMenu.addAction(act)

    def update_image(self):
        if self.vs is None:
            frame = self.show_frame
        else:
            ret, frame = self.vs.read()
        if not ret:
            if self.video is None:
                self.vs = cv2.VideoCapture(0)
            else:
                self.vs = cv2.VideoCapture(self.video)
            ret, frame = self.vs.read()
        packet = self.processor.getProcessedImage(frame)

        if len(packet['graph_values']):
            self.graph_values = packet['graph_values']

        face = packet['faces']
        if not face is None:
            if self.video is None:
                name = '梁瑛平'
            else:
                name = '彭于晏'
            print('【检测成功】')
            value = {KEYS.CARID: name,
                     KEYS.CARIMAGE: QPixmap.fromImage(self.toQImage(face)),
                     KEYS.CARCOLOR: '男',
                     KEYS.LICENSEIMAGE: None,
                     KEYS.LICENSENUMBER: '1120182525',
                     KEYS.LOCATION: '待输入',
                     KEYS.RULENAME: '待输入',
                     KEYS.RULEID: '待输入'}
            self.updateLog(value)

        qimg = self.toQImage(resize(packet['frame'], height=300))
        self.live_preview.setPixmap(QPixmap.fromImage(qimg))

    def updateCamInfo(self, feed):
        if feed is None:
            self.vs = None
        else:
            if isinstance(feed, str):
                feed = eval(feed)
            self.vs = cv2.VideoCapture(feed)
        self.processor = MainProcessor()

    def toQImage(self, raw_img):

        img = copy(raw_img)
        qformat = QImage.Format_Indexed8
        if len(img.shape) == 3:
            if img.shape[2] == 4:
                qformat = QImage.Format_RGBA8888
            else:
                qformat = QImage.Format_RGB888

        outImg = QImage(
            img.tobytes(), img.shape[1], img.shape[0], img.strides[0], qformat)
        outImg = outImg.rgbSwapped()
        return outImg

    def updateLog(self, data):

        listWidget = ViolationItem()
        listWidget.setData(data)
        listWidgetItem = QtWidgets.QListWidgetItem(self.cars_list)
        listWidgetItem.setSizeHint(listWidget.sizeHint())
        self.cars_list.addItem(listWidgetItem)
        self.cars_list.setItemWidget(listWidgetItem, listWidget)


    def start_tcp_server(self):
        return

    def tcp_close(self):
        return
