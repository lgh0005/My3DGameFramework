import gui_window as gw

## TODO
# 1. 경로 입력란 옆 Browse 버튼 오른쪽에 Clear 버튼 추가 고려 필요.
# 2. 이후에 subprocess에 하드 코딩된 timeout을 이용하는 것이 아니라
# 정말로 자식 프로세스(백엔드 C++)가 종료될 때까지 기다리는 방식으로 변경 필요.

app = gw.Window()

if __name__ == "__main__":
    app.run()
