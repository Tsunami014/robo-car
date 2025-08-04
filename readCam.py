import cv2
import cv2.aruco as aruco
import numpy as np

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
    print(ids)
    if np.all(ids):
        image = aruco.drawDetectedMarkers(frame,corners,ids)         
        cv2.imshow('Display', image)
    else:
        display = frame
        cv2.imshow('Display', display)
    if cv2.waitKey(1) & 0xFF == ord('q'): # Press 'q' to quit
        break

