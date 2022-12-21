from typing import Set
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.staticfiles import StaticFiles
import json

app = FastAPI()

class ConnectionManager:
    def __init__(self):
        self.active_connections: Set[WebSocket] = set()

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.add(websocket)

    def disconnect(self, websocket: WebSocket):
        self.active_connections.discard(websocket)

    async def message_to_self(self, message: str, websocket: WebSocket):
        msg = {
            "username": "self",
            "message": message
        }
        await websocket.send_json(msg)

    async def broadcast(self, sender: str, message: str):
        msg = {
            "username": sender,
            "message": message
        }
        for connection in self.active_connections:
            print(f"sending '{message}' to {connection.client.host}")
            await connection.send_json(msg)

    async def broadcast_except(self, sender: str, message: str, exception: WebSocket):
        msg = {
            "username": sender,
            "message": message
        }
        for connection in self.active_connections:
            if connection is not exception:
                print(f"sending '{message}' to {connection.client.host}")
                await connection.send_json(msg)

manager = ConnectionManager()

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await manager.connect(websocket)
    try:
        while True:
            msg = await websocket.receive_text()
            print(f"received '{msg}' from {websocket.client.host}")
            ip = websocket.client.host
            await manager.message_to_self(msg, websocket)
            await manager.broadcast_except(ip, msg, websocket)
    except WebSocketDisconnect:
        ip = websocket.client.host
        manager.disconnect(websocket)
        await manager.broadcast(ip, "[going dark]")

app.mount("/", StaticFiles(directory="public", html=True), name="public")

