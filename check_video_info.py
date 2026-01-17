import cv2
import sys

def get_video_info(filename):
    cap = cv2.VideoCapture(filename)
    
    print(f"\nVideo: {filename}")
    print(f"  Resolution: {int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))}x{int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))}")
    print(f"  FPS: {cap.get(cv2.CAP_PROP_FPS)}")
    print(f"  Total Frames: {int(cap.get(cv2.CAP_PROP_FRAME_COUNT))}")
    print(f"  FourCC: {int(cap.get(cv2.CAP_PROP_FOURCC))}")
    
    fourcc = int(cap.get(cv2.CAP_PROP_FOURCC))
    codec = "".join([chr((fourcc >> 8 * i) & 0xFF) for i in range(4)])
    print(f"  Codec: {codec}")
    
    bitrate = cap.get(cv2.CAP_PROP_BITRATE)
    if bitrate > 0:
        print(f"  Bitrate: {bitrate / 1_000_000:.2f} Mbps")
    
    cap.release()

if __name__ == "__main__":
    get_video_info("tests/data/input/Many.mp4")
    get_video_info("tests/data/output/Many_output.mp4")
