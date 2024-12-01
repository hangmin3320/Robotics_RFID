from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from fastapi.responses import HTMLResponse
from pydantic import BaseModel

app = FastAPI()

# 데이터를 저장할 리스트
data_storage = []

# Jinja2 템플릿 설정
templates = Jinja2Templates(directory="/Users/johangmin/Desktop/Dev/GCU/1/Robotics/RFID_Project/templates")


class RFIDData(BaseModel):
    id: str
    name: str
    received_at: str
    access_granted: bool  # 새로운 필드 추가


@app.post("/receive-data/")
async def receive_data(data: RFIDData):
    print("Received data:", data)
    data_storage.append(data)
    return {"message": "Data received successfully", "status code": 200}


@app.get("/get-data/", response_class=HTMLResponse)
async def get_data(request: Request):
    # 각 RFIDData 객체를 딕셔너리로 변경
    data_to_render = [data.dict() for data in data_storage]
    return templates.TemplateResponse("data.html", {"request": request, "data": data_to_render})