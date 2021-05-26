import dlib
import os
import cv2
import math
import imutils
import numpy as np
import time

from func.SleepDetection.Drowsiness_Detection import SleepDetector
from func.HRDetection.hrdetector import HeartRateDetector

im = None
result = None

detector = dlib.get_frontal_face_detector()

class FaceTracker(object):

    def __init__(self):

        self.sleep_detector = SleepDetector()
        self.hr_detector = HeartRateDetector(detector)

        self.fps = 0

        self.carTracker = {}
        self.carNumbers = {}
        self.carLocation1 = {}
        self.carLocation2 = {}
        self.carIllegals = []
        self.carDirections = None

        self.recorded = []

    def feedCap(self, frame):

        retDict = {
            'frame': None,
            'faces': None,
            'graph_values': [],
            'eye_values': None
        }

        ImageH, ImageW = frame.shape[:2]

        _, eye_ratio = self.sleep_detector.detect_sleep(frame.copy())
        hr_graph, face_roi = self.hr_detector.detect(frame)
        if not face_roi is None:
            retDict['faces'] = face_roi
        image = imutils.resize(frame, width=hr_graph.shape[1])

        retDict['graph_values'] = self.hr_detector.graph_values
        retDict['eye_values'] = eye_ratio
        retDict['frame'] = np.vstack([image, hr_graph])

        return retDict
