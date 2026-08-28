from dataclasses import dataclass


@dataclass
class Detection:
    """一個物件偵測結果。"""
    x1: float
    y1: float
    x2: float
    y2: float
    confidence: float




class Frame:
    """
    Camera 拍下的一張影像。
    輸入：id（int），size（bytes 數，模擬 raw pixel buffer 大小）
    儲存：self.id, self.data（bytes 物件）
    """
    def __init__(self, id: int, size: int):
        self.id = id
        self.size = size
        self.data = bytes(size)


class Camera:
    """
    模擬相機，每次 capture() 產生一張新 Frame。
    輸入：frame_size（bytes，預設 640*640*3 = 1,228,800）
    輸出：capture() 回傳 Frame，id 從 0 開始每次 +1
    """
    def __init__(self, frame_size: int = 640 * 640 * 3):
        self.frame_size = frame_size   # 存進 self
        self.counter = 0               # id counter 從 0 開始



    def capture(self) -> Frame:
        f = Frame(id=self.counter, size=self.frame_size)
        self.counter = self.counter + 1
        return f



class InferenceEngine:
    """
    模擬 YOLO inference。
    輸入：run(frame: Frame)
    輸出：list[Detection]，固定回傳 2 個假偵測結果
    """
    def run(self, frame: Frame) -> list[Detection]:
        return [
            Detection(x1=0.1, y1=0.2, x2=0.5, y2=0.8, confidence=0.9),
            Detection(x1=0.3, y1=0.1, x2=0.7, y2=0.6, confidence=0.75),
        ]





def main():
    camera = Camera()
    engine = InferenceEngine()

    for _ in range(5):
        frame = camera.capture()
        detections = engine.run(frame)
        print(f"frame {frame.id}: {len(detections)} detections, "
              f"buffer size={len(frame.data)} bytes")
        for d in detections:
            print(f"  bbox=({d.x1:.1f},{d.y1:.1f},{d.x2:.1f},{d.y2:.1f}) "
                  f"conf={d.confidence:.2f}")


if __name__ == "__main__":
    main()
