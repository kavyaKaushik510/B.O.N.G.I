import cv2
import requests
import numpy as np
import os
from datetime import datetime

# ===============================
# CONFIGURATION
# ===============================

ESP32_URL = 'http://192.168.3.64/capture'  # URL of ESP32-CAM image capture endpoint
frame_width = 640                         # Desired frame width
frame_height = 480                        # Desired frame height
fps = 10                                  # Frames per second for the recorded video

# ===============================
# SETUP OUTPUT DIRECTORY & FILENAME
# ===============================
folder = "recorded_streams"               # Folder to store recorded videos
os.makedirs(folder, exist_ok=True)        # Create folder if it doesn't exist

# Generate a unique filename using timestamp
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
output_filename = os.path.join(folder, f"stream_{timestamp}.mp4")

# ===============================
# INITIALIZE VIDEO WRITER
# ===============================
fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # Video codec for MP4 format
out = cv2.VideoWriter(output_filename, fourcc, fps, (frame_width, frame_height))

# Use persistent session to reduce connection overhead
session = requests.Session()

# ===============================
# CONFIGURE DISPLAY WINDOW
# ===============================
cv2.namedWindow("Recording ESP32 Stream", cv2.WINDOW_NORMAL)  # Create resizable window
cv2.resizeWindow("Recording ESP32 Stream", frame_width, frame_height)  # Set window size

print("Recording stream... Press 'q' to stop and save.")

# ===============================
# MAIN STREAM LOOP
# ===============================
while True:
    try:
        # Request a single JPEG image from the ESP32-CAM
        response = session.get(ESP32_URL, timeout=5)
        if response.status_code != 200:
            print("Failed to get image")
            continue

        # Convert byte content to a NumPy array, then decode to image
        img_array = np.frombuffer(response.content, dtype=np.uint8)
        frame = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

        if frame is None:
            print("Failed to decode image")
            continue

        # Resize the image to match output video size
        frame = cv2.resize(frame, (frame_width, frame_height))

        # Write the frame to video file
        out.write(frame)

        # Display the live video frame
        cv2.imshow("Recording ESP32 Stream", frame)

        # Stop recording if user presses 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    except Exception as e:
        print(f"Error: {e}")
        break

# ===============================
# CLEANUP
# ===============================
out.release()                 # Finalize video file
cv2.destroyAllWindows()       # Close OpenCV window
print(f"Video saved as {output_filename}")
