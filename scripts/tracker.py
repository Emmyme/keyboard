import time
import json
import socket

import cv2
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision

PORT = 51234
MODEL = "assets/models/hand_landmarker.task"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
addr = ("127.0.0.1", PORT)

options = vision.HandLandmarkerOptions(
    base_options=python.BaseOptions(model_asset_path=MODEL),
    num_hands=2,
    running_mode=vision.RunningMode.VIDEO,
    min_hand_detection_confidence=0.6,
    min_tracking_confidence=0.5,
)
landmarker = vision.HandLandmarker.create_from_options(options)

def open_camera():
    # Try the default backend first, then explicit Windows backends. Some webcams
    # and OpenCV builds fail on DSHOW/MSMF by index but work on the default.
    for index, backend in [(0, cv2.CAP_ANY), (0, cv2.CAP_MSMF), (0, cv2.CAP_DSHOW), (1, cv2.CAP_ANY)]:
        cap = cv2.VideoCapture(index, backend)
        if cap.isOpened():
            ok, _ = cap.read()
            if ok:
                print(f"tracker: camera opened (index {index}, backend {backend})", flush=True)
                return cap
        cap.release()
    return None

cap = open_camera()
if cap is None:
    print("tracker: ERROR no camera could be opened on any backend", flush=True)
    raise SystemExit(1)
print(f"tracker: streaming hand landmarks to udp://127.0.0.1:{PORT}", flush=True)

start = time.time()
last_ts = -1
while True:
    ok, frame = cap.read()
    if not ok:
        continue
    frame = cv2.flip(frame, 1)  # selfie mirror (matches the overlay's convention)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)

    ts = int((time.time() - start) * 1000)
    if ts <= last_ts:  # detect_for_video needs strictly increasing timestamps
        ts = last_ts + 1
    last_ts = ts

    result = landmarker.detect_for_video(mp_image, ts)
    out = {"hands": []}
    for i, hlm in enumerate(result.hand_landmarks):
        lm = []
        for p in hlm:
            lm += [round(p.x, 4), round(p.y, 4), round(p.z, 4)]
        hd = 0
        if result.handedness and i < len(result.handedness):
            hd = 1 if result.handedness[i][0].category_name == "Right" else 0
        out["hands"].append({"lm": lm, "hd": hd})

    try:
        sock.sendto(json.dumps(out).encode("utf-8"), addr)
    except OSError:
        pass
