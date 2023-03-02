# SETUP
### mosquittoのインストール
### 参考URL：https://mosquitto.org/
```bash
sudo apt-get update
sudo apt-get -y install mosquitto mosquitto-clients apache2
```

### mosquitto.confファイルの先頭に，以下の3行を追加
```bash
sudo -e /etc/mosquitto/mosquitto.conf
```

```conf
listener  1883 # MQTTを1883番ポートで受信する
listener 15675 # WebSocketを15675番ポートで受信する
protocol websockets
allow_anonymous true    # ubuntuでやるときはこれが必要。raspberry piでは不要だった。
```

### 自動起動の設定とサービスの再起動
```bash
sudo systemctl enable mosquitto.service
sudo systemctl restart mosquitto
```

### node.jsとnpmのインストール
```bash
sudo apt-get install nodejs
sudo apt-get install npm
```

### node modulesの設定
```bash
npm ci
```

# OMS serverのセットアップ
### 参考URL：https://switch2osm.org/serving-tiles/manually-building-a-tile-server-ubuntu-22-04-lts/

# シリアルの有効化(ttyACM0かttyUSB0のどちらか)
```bash
sudo chmod 666 /dev/ttyACM0
sudo chmod 666 /dev/ttyUSB0
```

### socket 環境構築
```bash
# installjsoncpp
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