import gui_window as gw

# TODO : 경로에 한국어가 있는 경우 컨버팅에 실패하는 경우가 있음
# 프로세스가 텍스쳐를 ktx로 구울 때 하나씩 동기적으로 굽다보니 너무 느림
# 이는 비동기로 돌아가도록 만들 필요가 다분함

if __name__ == "__main__":
    app = gw.Window()
    app.run()
