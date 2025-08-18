import cv2
import cv2.aruco as aruco
import numpy as np
import math

# TODO: Fix flickering

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
    # Fake calibration
    h, w = frame.shape[:2]
    focal_length = w
    cameraMatrix = np.array([[focal_length, 0, w/2],
                             [0, focal_length, h/2],
                             [0, 0, 1]], dtype=np.float32)
    distCoeffs = np.zeros(5)  # Assume no distortion

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    corners, ids, rejectedImgPoints = detector.detectMarkers(gray)
    if ids is not None:
        rvecs, tvecs = estimate_pose(corners, marker_length, cameraMatrix, distCoeffs)

        for i in range(len(ids)):
            # Draw axis for each marker
            frame = cv2.drawFrameAxes(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], marker_length*0.5)

            """# Print 3D position
            x, y, z = tvecs[i]
            x, y, z = x[0], y[0], z[0]
            p, r, y = rvecs[i]  # Pitch Roll Yaw
            p, r, y = p[0], r[0], y[0]
            print(f"Marker ID {ids[i][0]} position: x={x:.2f}, y={y:.2f}, z={z:.2f}, pitch={p:.2f}, roll={r:.2f}, yaw={y:.2f}")"""

        """for idx in range(len(ids)):
            id, cns = ids[idx], corners[idx]
            cv2.fillPoly(frame, cns.astype(np.int32), (250, 250, 250))
            centre = np.array((
                sum(i[0] for i in cns[0])/len(cns[0]),
                sum(i[1] for i in cns[0])/len(cns[0])
            ), dtype=np.int64)
            std = (find_std([i[0] for i in cns[0]]), find_std([i[1] for i in cns[0]]))
            mstd = max(std)*2
            nbanana = cv2.resize(banana, (mstd, mstd))
            h, w, _ = nbanana.shape
            overlay_img(frame, nbanana, centre[0]-w//2, int(centre[1]-h/4*3))"""
    cv2.imshow('Display', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): # Press 'q' to quit
        break

