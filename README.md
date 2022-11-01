# Smart-Tumbler
경기산업기술교육센터] 개인 프로젝트 2 - Smart Tumbler

- **개발 필요성 :** 일반적인 텀블러에 Hot/Ice과 위치 정보 등의 다양한 기능을 추가 하여 사용자에게 텀블러가 더욱더 활용 될수 있게 개발 하였습니다.

- **개발 목표 :**

       ① 텀블러 안 물의 양 Array LED로 표현
       ② 텀블러 분실 시 Blue Tooth를 통한 GPS 위치정보 스마트폰 실시간 확인
       ③ 텀블러가 넘어질 시 Buzzer를 통한 알림
       ④ 텀블러 내부 온도 3색 LED로 표현
       ⑤ 텀블러 내부 물이 차가울 시 수 냉/온 쿨러를 통해 뜨겁게 함

- **개발 기간 :** 2016. 8. 3 ~ 2016. 9. 30 (약 2개월)
- **개발 언어 및 환경 :** C/C++ / Windows 7 (64bit) / Atmel Studio 7.0 / MCU(Atmega128)
- **개발 내용 :**

       1) **GPS Sensor :** 텀블러의 위치 정보 실시간 확인

       2) **Blue Tooth :** MCU 인 Atmega 128 과 스마트폰 간 통신

       3) **Octopus Soil Moisture Sensor :** 텀블러 안 수위를 측정 하여 Analog 출력

       4) **Buzzer :** 텀블러의 위치, 각도에 따른 알림(텀블러 넘어 질 경우)

       5) **Gyro Sensor :** 텀블러의 3축 각도를 측정

       6) **Array LED :** 텀블러 안 물의 양 표시

       7) **3색 LED :** 텀블러 안 물의 온도 표시

- **구현 기술 :**

       1) **Octopus Soil Moisture Sensor : 수위 측정 센서, ADC**

           □  용도
                ▶ 아날로그 센서인 수위 센서의 값 측정
           □  기술내용
                ▶ 10비트 축차형 ADC를 이용한 아날로그 센서값 입력
                ▶ ADC로 입력된 전압값을 물의 양으로 변환하는 기술
                ▶ 수위센서와 MCU간 인터페이스

        2) **Blue Tooth : Blue Tooth, UART Interface**

           □ 용도
                ▶ 블루투스 모듈을 사용하여 GPS Sensor의 위치정보를 스마트폰으로 실시간 확인
           □ 기술내용
                ▶ 블루투스 모듈과 MCU간 인터페이스
                ▶ Interrupt를 이용한 양방향 UART 통신 프로그램 작성

        3) **GPS Sensor : GPS, UART Interface**

           □ 용도
               ▶ GPS Sensor을 사용하여 텀블러의 위치정보를 MCU에 전송
           □ 기술내용
               ▶ GPS Sensor와 MCU간 인터페이스
               ▶ Interrupt를 이용한 양방향 UART 통신 프로그램 작성

        4) **Gyro Sensor : 3축 각도, 가속도 측정 센서, I2C Interface**

           □ 용도
              ▶ I2C 기능을 이용하여 Gyro Sensor가 측정한 Gyro x, y, z 3축값과 온도 값을 MCU로 전송
           □ 기술내용
              ▶ 디지털 센서와 MCU간 인터페이스에 많이 사용되고 있는 I2C 인터페이스 구현
              ▶ I2C 기능을 지원하는 센서 또는 칩 활용 기술 (비트 단위 직렬 전송 구현)
