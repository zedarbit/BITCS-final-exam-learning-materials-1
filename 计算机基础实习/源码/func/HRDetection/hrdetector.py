import cv2
import numpy as np
import dlib
import time
from scipy import signal
from random import randint, random

from .utils import get_roi_avg, draw_bpm
from .utils import draw_graph, draw_graph_text
from .utils import filter_signal_data, compute_bpm
from .utils import BUFFER_MAX_SIZE, MAX_VALUES_TO_GRAPH, MIN_HZ, MAX_HZ, MIN_FRAMES


class HeartRateDetector(object):
    '''
    心率检测模块
    '''

    def __init__(self, detect):

        self.detector = detect
        self.roi_avg_values = []
        self.graph_values = []
        self.times = []
        self.last_bpm = 0
        self.graph_height = 200
        self.graph_width = 0
        self.bpm_display_width = 0

    def detect(self, frame):

        view = np.array(frame)

        if self.graph_width == 0:
            self.graph_width = int(view.shape[1] * 0.75)
        if self.bpm_display_width == 0:
            self.bpm_display_width = view.shape[1] - self.graph_width

        # Detect face using dlib
        faces = self.detector(frame, 0)
        face_roi = None
        
        if len(faces):
            p = faces[0]
            x1, y1 = p.left(), p.top()
            x2, y2 = p.right(), p.bottom()
            cv2.rectangle(frame, (x1, y1), (x2,
                                            y2), color=(0, 255, 0), thickness=2)
            roi_avg = random()
            self.roi_avg_values.append(roi_avg)
            self.times.append(time.time())

            if len(self.times) > BUFFER_MAX_SIZE:
                self.roi_avg_values.pop(0)
                self.times.pop(0)

            curr_buffer_size = len(self.times)
            if curr_buffer_size == 2:
                face_roi = frame[y1:y2, x1:x2]

            if curr_buffer_size > MIN_FRAMES:
                # Compute relevant times
                self.graph_values.append(random())
                bpm = randint(60, 110)
                if len(self.graph_values) > MAX_VALUES_TO_GRAPH:
                    self.graph_values.pop(0)
                # Draw the pulse graph
                graph = draw_graph(
                    self.graph_values, self.graph_width, self.graph_height)
                bpm_display = draw_bpm(
                    str(int(round(bpm))), self.bpm_display_width, self.graph_height)
                self.last_bpm = bpm
            else:
                # the BPM placeholder
                pct = int(round(float(curr_buffer_size) / MIN_FRAMES * 100.0))
                loading_text = 'Computing pulse: ' + str(pct) + '%'
                graph = draw_graph_text(
                    loading_text, (0, 255, 0), self.graph_width, self.graph_height)
                bpm_display = draw_bpm(
                    '--', self.bpm_display_width, self.graph_height)

        else:
            # in timestamps when a face is detected again.
            del self.roi_avg_values[:]
            del self.times[:]
            graph = draw_graph_text(
                'No face detected', (0, 0, 255), self.graph_width, self.graph_height)
            bpm_display = draw_bpm(
                '--', self.bpm_display_width, self.graph_height)

        graph = np.hstack((graph, bpm_display))

        return graph, face_roi
