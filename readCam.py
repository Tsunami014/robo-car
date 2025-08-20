import cv2
import cv2.aruco as aruco
import numpy as np
import math
from collections import defaultdict


DAMPENING = 0.0003  # The dampening from the movement predictions.
# TODO: Intorpelate between values when going off predictions

cameraMatrix, distCoeffs = None, None
# The difference every frame
x, y, z = 0, 0, 0
p, r, yw = 0, 0, 0
class Tag:
    Timeout = 30
    def __init__(self):
        self.called = False
        self.untilDeath = self.Timeout
        self.pos = (0, 0, 0)
        self.rot = (0, 0, 0)
        self.sze = 0

    def __call__(self, found, *args):
        if self.called:
            return
        self.called = True
        if found:
            self.pos, self.rot, self.sze = args[:3]
            self.untilDeath = self.Timeout
        else:
            self.untilDeath -= 1
            if self.untilDeath <= 0:
                idx = list(tagsL.values()).index(self)
                id = list(tagsL.keys())[idx]
                tagsL.pop(id)
                return False
            self.pos = (self.pos[0]+x*DAMPENING, self.pos[1]+y*DAMPENING, self.pos[2]+z*DAMPENING)
            self.rot = (self.rot[0]+p*DAMPENING, self.rot[1]+r*DAMPENING, self.rot[2]+yw*DAMPENING)

    def draw(self, frame):
        ps, _ = cv2.projectPoints(np.array([[[0, 0, 0]]], dtype=np.float32), np.array(self.rot, dtype=np.float32), np.array(self.pos, dtype=np.float32), cameraMatrix, distCoeffs)
        p = ps[0][0]
        nbanana = cv2.resize(banana, (self.sze, self.sze))
        h, w, _ = nbanana.shape
        overlay_img(frame, nbanana, int(p[0]-w/2), int(p[1]-h/4*3))

tagsL = defaultdict(Tag)

marker_length = 0.0036  # Marker size in meters

banana = cv2.imread('Banana.png', cv2.IMREAD_UNCHANGED)

def overlay_img(background, foreground, x_offset, y_offset):
    # normalize alpha channels from 0-255 to 0-1
    alpha_channel = foreground[:, :, 3] / 255.0
    bgr_channels = foreground[:, :, :3]
    bgh, bgw, _ = background.shape
    h, w, _ = foreground.shape
    x_offset, y_offset = max(min(x_offset, bgw-w), 0), max(min(y_offset, bgh-h), 0)
    for c in range(0, 3):
        background[y_offset:y_offset+h, x_offset:x_offset+w, c] = \
            (alpha_channel * bgr_channels[:, :, c] +
             (1 - alpha_channel) * background[y_offset:y_offset+h, x_offset:x_offset+w, c])

def estimate_pose(corners, marker_size, camera_matrix, dist_coeffs):
    # Define 3D coordinates of marker corners in marker's own coordinate system
    obj_points = np.array([
        [-marker_size / 2,  marker_size / 2, 0],
        [ marker_size / 2,  marker_size / 2, 0],
        [ marker_size / 2, -marker_size / 2, 0],
        [-marker_size / 2, -marker_size / 2, 0]
    ], dtype=np.float32)

    rvecs, tvecs = [], []
    for corner in corners:
        success, rvec, tvec = cv2.solvePnP(obj_points, corner[0], camera_matrix, dist_coeffs)
        if success:
            rvecs.append(rvec)
            tvecs.append(tvec)
    return rvecs, tvecs

def find_std(pts):
    mean = sum(pts)/len(pts)
    return round(math.sqrt(sum(pow(i-mean, 2) for i in pts)/len(pts)))

def fmt(x):
    return f'{x:.2f}'.rjust(5)

cap = cv2.VideoCapture("http://192.168.4.1:81/stream")
if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()
aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_6X6_50)
arucoParameters = aruco.DetectorParameters()
detector = aruco.ArucoDetector(aruco_dict, arucoParameters)
prevFrame = None
while True:
    ret, frame = cap.read()
    if not ret:
        print("End of stream or error reading frame.")
        break
    # Fake calibration
    h, w = frame.shape[:2]
    focal_length = w
    cameraMatrix = np.array([[focal_length, 0, w/2],
                             [0, focal_length, h/2],
                             [0, 0, 1]], dtype=np.float32)
    distCoeffs = np.zeros(5)  # Assume no distortion

    # Unset called in every tag
    for t in tagsL:
        tagsL[t].called = False

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    corners, ids, rejectedImgPoints = detector.detectMarkers(gray)
    # Loop over found tags
    if ids is not None:
        rvecs, tvecs = estimate_pose(corners, marker_length, cameraMatrix, distCoeffs)

        for i, id in enumerate(ids):
            id = id[0]
            cns = corners[i]
            x1, y1, z1 = tvecs[i]
            x1, y1, z1 = x1[0], y1[0], z1[0]
            p1, r1, yw1 = rvecs[i]  # Pitch Roll Yaw
            p1, r1, yw1 = p1[0], r1[0], yw1[0]

            centre = np.array((
                sum(i[0] for i in cns[0])/len(cns[0]),
                sum(i[1] for i in cns[0])/len(cns[0])
            ), dtype=np.int64)
            std = (find_std([i[0] for i in cns[0]]), find_std([i[1] for i in cns[0]]))
            mstd = max(std)*2

            tagsL[id](True, (x1, y1, z1), (p1, r1, yw1), mstd)

    x, y, z = 0, 0, 0
    p, r, yw = 0, 0, 0
    if prevFrame is not None:
        orb = cv2.ORB_create()
        kp1, des1 = orb.detectAndCompute(frame, None)
        kp2, des2 = orb.detectAndCompute(prevFrame, None)

        if des1 is not None and des2 is not None:
            matcher = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
            matches = matcher.match(des1, des2)
            
            pts1 = np.float32([kp1[m.queryIdx].pt for m in matches])
            pts2 = np.float32([kp2[m.trainIdx].pt for m in matches])

            try:
                E, mask = cv2.findEssentialMat(pts1, pts2, cameraMatrix)
                _, R, t, mask = cv2.recoverPose(E, pts1, pts2, cameraMatrix)
                done = True
            except Exception:
                done = False
            if done:
                x, y, z = t
                x, y, z = x[0], y[0], z[0]
                p, r, yw = R  # Pitch Roll Yaw
                p, r, yw = p[0], r[0], yw[0]
                p = p-1

    for t in tagsL.copy():
        if tagsL[t](False) is not False:
            tagsL[t].draw(frame)

    prevFrame = frame
    cv2.imshow('Display', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): # Press 'q' to quit
        break

