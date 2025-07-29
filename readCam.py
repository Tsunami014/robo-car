import cv2

cap = cv2.VideoCapture("http://192.168.4.1:81/stream")
if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()
while True:
    ret, frame = cap.read() 
    if not ret:
        print("End of stream or error reading frame.")
        break
    cv2.imshow('Video Stream', frame) # Display the frame
    if cv2.waitKey(1) & 0xFF == ord('q'): # Press 'q' to quit
        break

