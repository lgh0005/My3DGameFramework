import gui_window as gw

## TODO
# 2. 이후에 subprocess에 하드 코딩된 timeout을 이용하는 것이 아니라
# 정말로 자식 프로세스(백엔드 C++)가 종료될 때까지 기다리는 방식으로 변경 필요.
# 3. 로딩 바와 같은 UI 추가가 필요. (이것도 사실은 백엔드에서 프로세스 상태를 알려줘야 구현 가능)
# 3. Avatar를 뽑아오는 GUIContext도 추가 필요. : 뼈의 위치를 바이너리 파일로 뽑아오는 기능.

app = gw.Window()

if __name__ == "__main__":
    app.run()
