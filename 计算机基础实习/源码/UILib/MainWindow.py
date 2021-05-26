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
from PyQt5 import QtWidgets
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QMainWindow, QListWidget, QAction, qApp, QMenu, QVBoxLayout
from PyQt5.uic import loadUi
import pyqtgraph as pg
from PyQt5.QtWidgets import QSizePolicy
from processor.MainProcessor import MainProcessor
from func.ip_utils import click_get_ip
from numpy import copy
import numpy as np
from UILib.smallWin import ViolationItem
from UILib.Database import KEYS


class MainWindow(QMainWindow):
    def __init__(self):

        super(MainWindow, self).__init__()
        loadUi("./data/UI/MainWindow.ui", self)

        self.show_frame = cv2.imread('data/show.jpg')
        self.cam_clear_gaurd = False
        self.myip = None
        self.face_id = None

        cam_groups = ['TCP服务端']

        self.camera_group.clear()
        self.group_list = [name for name in cam_groups]
        self.camera_group.addItems(self.group_list)
        self.camera_group.setCurrentIndex(0)

        cams = [('初始界面', '', None, '待添加')]

        self.cam_selector.clear()
        self.cam_selector.addItems(name for name, location, feed, func in cams)
        self.cam_selector.setCurrentIndex(0)

        self.log_tabwidget.clear()
        self.cars_list = QListWidget(self)
        self.illegal_list = QListWidget(self)
        self.log_tabwidget.addTab(self.cars_list, "患者记录")
        self.log_tabwidget.addTab(self.illegal_list, "危险记录")

        self.initMenu()
        self.initParams()

        self.connect_net1.clicked.connect(self.start_tcp_server)
        self.connect_net2.clicked.connect(self.start_tcp_server)
        self.connect_net3.clicked.connect(self.start_tcp_server)

        self.close_net1.clicked.connect(lambda: self.close_tcp_server(index=0))
        self.close_net2.clicked.connect(lambda: self.close_tcp_server(index=1))
        self.close_net3.clicked.connect(lambda: self.close_tcp_server(index=2))

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.plotData)
        self.timer.start(50)

        self.feed = None
        self.vs = None
        self.graph_values = [[], [], []]
        self.eye_values = [[], [], []]
        self.updateCamInfo(None)

    def initParams(self):

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
        act.triggered.connect(qApp.quit)
        fileMenu.addAction(act)

    def updateCamInfo(self, feed):
        if feed is None:
            self.vs = None
        else:
            if isinstance(feed, str):
                feed = eval(feed)
            self.vs = cv2.VideoCapture(feed)
        self.processor = MainProcessor(self.cam_selector.currentText())

    def updateLog(self, data):

        listWidget = ViolationItem()
        listWidget.setData(data)
        listWidgetItem = QtWidgets.QListWidgetItem(self.cars_list)
        listWidgetItem.setSizeHint(listWidget.sizeHint())
        self.cars_list.addItem(listWidgetItem)
        self.cars_list.setItemWidget(listWidgetItem, listWidget)

    def generate_image(self):
        
        verticalLayout = QVBoxLayout(self.hr_preview1)
        win = pg.GraphicsLayoutWidget(self.hr_preview1)
        verticalLayout.addWidget(win)
        p = win.addPlot(title="动态波形图")
        p.showGrid(x=True, y=True)
        p.setLabel(axis="left", text="Amplitude / V")
        p.setLabel(axis="bottom", text="t / s")
        p.setTitle("心率")
        p.addLegend()
        self.curve11 = p.plot(pen="g", name="y1")
        self.curve12 = p.plot(pen="r", name="y2")

        verticalLayout = QVBoxLayout(self.hr_preview2)
        win = pg.GraphicsLayoutWidget(self.hr_preview2)
        verticalLayout.addWidget(win)
        p = win.addPlot(title="动态波形图")
        p.showGrid(x=True, y=True)
        p.setLabel(axis="left", text="Amplitude / V")
        p.setLabel(axis="bottom", text="t / s")
        p.setTitle("心率")
        p.addLegend()
        self.curve21 = p.plot(pen="g", name="y1")
        self.curve22 = p.plot(pen="r", name="y2")

        verticalLayout = QVBoxLayout(self.hr_preview3)
        win = pg.GraphicsLayoutWidget(self.hr_preview3)
        verticalLayout.addWidget(win)
        p = win.addPlot(title="动态波形图")
        p.showGrid(x=True, y=True)
        p.setLabel(axis="left", text="Amplitude / V")
        p.setLabel(axis="bottom", text="t / s")
        p.setTitle("心率")
        p.addLegend()
        self.curve31 = p.plot(pen="g", name="y1")
        self.curve32 = p.plot(pen="r", name="y2")

    def plotData(self):

        if len(self.graph_values[0]):
            self.curve11.setData(np.arange(len(self.graph_values[0])),
                                np.array(self.graph_values[0]))
            self.curve12.setData(np.arange(len(self.eye_values[0])),
                                np.array(self.eye_values[0]))
        if len(self.graph_values[1]):
            self.curve21.setData(np.arange(len(self.graph_values[1])),
                                np.array(self.graph_values[1]))
            self.curve22.setData(np.arange(len(self.eye_values[1])),
                                np.array(self.eye_values[1]))
        if len(self.graph_values[2]):
            self.curve31.setData(np.arange(len(self.graph_values[2])),
                                np.array(self.graph_values[2]))
            self.curve32.setData(np.arange(len(self.eye_values[2])),
                                np.array(self.eye_values[2]))

    def update_values_1(self, values):
        try:
            if len(values):
                values = values.split(',')
                self.graph_values[0].append(eval(values[0]))
                self.eye_values[0].append(eval(values[1]))
                if values[2] == '0':
                    self.face_pt = None
                else:
                    self.face_pt = values[2]
                    self.updateSeverLog(self.face_pt)
            if len(self.graph_values[0]) > self.values_max_num:
                self.graph_values[0].pop(0)
            if len(self.eye_values[0]) > self.values_max_num:
                self.eye_values[0].pop(0)
        except Exception as e:
            print(e)
            self.my_thread[0].quit()

    def update_values_2(self, values):
        try:
            if len(values):
                values = values.split(',')
                self.graph_values[1].append(eval(values[0]))
                self.eye_values[1].append(eval(values[1]))
                if values[2] == '0':
                    self.face_pt = None
                else:
                    self.face_pt = values[2]
                    self.updateSeverLog(self.face_pt)
            if len(self.graph_values[1]) > self.values_max_num:
                self.graph_values[1].pop(0)
            if len(self.eye_values[1]) > self.values_max_num:
                self.eye_values[1].pop(0)
        except Exception as e:
            print(e)
            self.my_thread[1].quit()

    def update_values_3(self, values):
        try:
            if len(values):
                values = values.split(',')
                self.graph_values[2].append(eval(values[0]))
                self.eye_values[2].append(eval(values[1]))
                if values[2] == '0':
                    self.face_pt = None
                else:
                    self.face_pt = values[2]
                    self.updateSeverLog(self.face_pt)
            if len(self.graph_values[2]) > self.values_max_num:
                self.graph_values[2].pop(0)
            if len(self.eye_values[2]) > self.values_max_num:
                self.eye_values[2].pop(0)
        except Exception as e:
            print(e)
            self.my_thread[2].quit()

    def updateSeverLog(self, face_pt):
        print('【检测成功】')
        value = {KEYS.CARID: '梁瑛平',
                 KEYS.CARIMAGE: QPixmap(face_pt),
                 KEYS.CARCOLOR: '男',
                 KEYS.LICENSEIMAGE: None,
                 KEYS.LICENSENUMBER: '1120182525',
                 KEYS.LOCATION: str(self.camera_group.currentText()),
                 KEYS.RULENAME: '待输入',
                 KEYS.RULEID: '待输入'}
        self.updateLog(value)

    def start_tcp_server(self, index=None):
        return

    def close_tcp_server(self):
        return
