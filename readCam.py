import cv2
import cv2.aruco as aruco
import numpy as np
import math

# TODO: Fix flickering

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

def find_std(pts):
    mean = sum(pts)/len(pts)
    return round(math.sqrt(sum(pow(i-mean, 2) for i in pts)/len(pts)))

cap = cv2.VideoCapture("http://192.168.4.1:81/stream")
if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()
aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_6X6_50)
arucoParameters = aruco.DetectorParameters()
detector = aruco.ArucoDetector(aruco_dict, arucoParameters)
while True:
    ret, frame = cap.read() 
    if not ret:
        print("End of stream or error reading frame.")
        break
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    corners, ids, rejectedImgPoints = detector.detectMarkers(gray)
    if ids is not None:
        for idx in range(len(ids)):
            id, cns = ids[idx], corners[idx]
            cv2.fillPoly(frame, cns.astype(np.int32), (250, 250, 250))
            centre = np.array((
                sum(i[0] for i in cns[0])/len(cns[0]),
                sum(i[1] for i in cns[0])/len(cns[0])
            ), dtype=np.int64)
            std = (find_std([i[0] for i in cns[0]]), find_std([i[1] for i in cns[0]]))
            nbanana = cv2.resize(banana, std)
            h, w, _ = nbanana.shape
            overlay_img(frame, nbanana, centre[0]-w//2, centre[1]-h//2)
    cv2.imshow('Display', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): # Press 'q' to quit
        break

