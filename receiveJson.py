import serial
import json
from datetime import datetime
import requests

DATA_DICT = {"entries": []}
PORT = '/dev/cu.usbmodem11101'
BAUDRATE = 9600
TIMEOUT = 1


def connect_to_serial_port(port, baudrate, timeout):
    try:
        connection = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        print("시리얼 포트 연결 성공!")
        return connection
    except serial.SerialException as e:
        print(f"시리얼 포트 연결 실패: {e}")
        return None


def get_current_timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def post_to_server(data):
    try:
        # 전송 전에 데이터 출력
        print("전송할 데이터:", data)
        response = requests.post("http://localhost:8000/receive-data/", json=data)
        if response.status_code == 200:
            print("데이터 전송 성공:", response.json())
        else:
            print("데이터 전송 실패:", response.status_code)
    except requests.exceptions.RequestException as e:
        print(f"요청 에러: {e}")


def read_and_parse_data(connection):
    try:
        raw_data = connection.readline().decode('utf-8').strip()
        if raw_data:
            print(f"수신된 데이터: {raw_data}")
            parsed_json = json.loads(raw_data)

            # 'access' 값에 따라 액세스 허가 여부 결정
            access_status = parsed_json.get("access", "Denied")
            access_granted = access_status == "Authorized"

            data_to_send = {
                "id": parsed_json.get("uid", "unknown"),
                "name": parsed_json.get("user", "unknown"),
                "received_at": get_current_timestamp(),
                "access_granted": access_granted  # 결정된 액세스 허가 여부 추가
            }

            post_to_server(data_to_send)  # 서버로 데이터 전송

    except json.JSONDecodeError:
        print("JSON 파싱 실패! 데이터 확인 필요.")


def main():
    arduino = connect_to_serial_port(PORT, BAUDRATE, TIMEOUT)
    if arduino:
        try:
            while True:
                read_and_parse_data(arduino)
        finally:
            arduino.close()


main()