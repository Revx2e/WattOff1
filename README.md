# ⚡ WattOff 에너지 지킴이

학교·집에서 **아무도 없는데 켜진 조명·에어컨**, **대기전력 콘센트**를 찾아 알려주고 원격으로 끄는 모바일 웹앱(PWA)입니다.

## 📱 설치 (갤럭시)
1. 이 저장소의 GitHub Pages 주소를 크롬으로 열기
2. 메뉴(⋮) → **홈 화면에 추가 / 앱 설치**

## 🚀 GitHub Pages 켜기
Settings → Pages → Branch: `main` / `(root)` → Save → 1~2분 뒤 `https://아이디.github.io/저장소이름/` 접속

## 📂 파일
| 파일 | 설명 |
|---|---|
| index.html | 앱 본체 |
| manifest.json | 앱 이름·아이콘 정보 |
| sw.js | 오프라인·알림용 서비스워커 |
| icon-192.png, icon-512.png | 앱 아이콘 |
| wattoff_classroom.ino | 교실용 ESP32 펌웨어 (Arduino IDE) |

## 🔌 실제 기기 연결
앱 설정 → **실제 기기 연결** → MQTT 브로커(wss) 주소·토픽 접두어 입력.
ESP32 코드의 `PREFIX`와 앱의 토픽 접두어를 똑같이 맞추세요.

⚠️ 220V 배선 직접 작업 금지 · KC 인증 제품 사용 · 설치 전 학교 승인 필수
