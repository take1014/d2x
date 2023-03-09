# client SETUP
## 1. mosquittoのインストール
### 1-1. Install mosquitto, mosquitto-clients  参考URL：https://mosquitto.org/
```bash
sudo apt-get update
sudo apt-get -y install mosquitto mosquitto-clients apache2
```

### 1-2. mosquitto.confファイルの先頭に，以下の3行を追加
```bash
sudo -e /etc/mosquitto/mosquitto.conf
```

```conf
listener  1883 # MQTTを1883番ポートで受信する
listener 15675 # WebSocketを15675番ポートで受信する
protocol websockets
allow_anonymous true    # ubuntuでやるときはこれが必要。raspberry piでは不要だった。
```

### 1-3. 自動起動の設定とサービスの再起動
```bash
sudo systemctl enable mosquitto.service
sudo systemctl restart mosquitto
```

## 2. node.jsの設定
### 2-1. Install nodejs, npm nvm
```bash
sudo apt-get install -y nodejs
sudo apt-get install -y npm
sudo apt-get install -y curl
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash
```

### 2-2. client起動に必要なnode_modulesのDL
```bash
cd d2x
npm ci
```

## 3. シリアルの有効化(ttyACM0かttyUSB0のどちらか)
```bash
sudo chmod 666 /dev/ttyACM0
sudo chmod 666 /dev/ttyUSB0
```

## 4. socket 環境構築
### 4-1. C++ でmqttを使用するために必要なライブラリインストール
```bash
# install jsoncpp
sudo apt-get install libjsoncpp-dev
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
# install mqtt
# install libraries
sudo apt-get install -y fakeroot devscripts dh-make lsb-release
sudo apt-get install -y libssl-dev
sudo apt-get install -y doxygen graphviz
# install paho.mqtt.c
cd ~
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
# install paho.mqtt.cpp
cd ~
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

### 4-2. Install logger
#### 参考：https://github.com/gabime/spdlog
```bash
cd ~
git clone https://github.com/gabime/spdlog.git
cd spdlog && mkdir build && cd build
cmake .. && make -j
sudo make install
sudo ldconfig
```

### 4-3.socket の実行
#### 4-3-1. C++
```bash
cd d2x/client/socket/cpp
mkdir build && cd build
cmake ..
make -j4
# 実行(GNSSデータをシリアルか読み込み補正→websocketでmqttを介してhtmlに送信)
./socket /path/to/app_conf.json
```

#### 4-3-2. js
```bash
cd d2x/client/socket/js
node gps.js # 事前にd2x以下でnpm ciを実行してmodulesをDLしておく
```