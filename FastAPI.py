from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from fastapi.responses import HTMLResponse
from pydantic import BaseModel

app = FastAPI()

# RFID 데이터 저장소
rfid_data_storage = []

# 템플릿 디렉터리 경로
TEMPLATES_DIRECTORY = "/Users/johangmin/Desktop/Dev/GCU/1/Robotics/RFID_Project/templates"

# Jinja2 템플릿 설정
templates = Jinja2Templates(directory=TEMPLATES_DIRECTORY)


class RFIDData(BaseModel):
    id: str
    name: str
    received_at: str
    access_granted: bool


@app.post("/receive-data/")
async def receive_data(data: RFIDData):
    log_received_data(data)
    store_data(data)
    return {"message": "Data received successfully", "status code": 200}


def log_received_data(data: RFIDData):
    print("Received data:", data)


def store_data(data: RFIDData):
    rfid_data_storage.append(data)


@app.get("/get-data/", response_class=HTMLResponse)
async def get_data(request: Request):
    data_to_render = [data.dict() for data in rfid_data_storage]
    return templates.TemplateResponse("data.html", {"request": request, "data": data_to_render})