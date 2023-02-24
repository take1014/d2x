# SETUP
### mosquittoのインストール
### 参考URL：https://mosquitto.org/
```bash
sudo apt-get update
sudo apt-get -y install mosquitto mosquitto-clients
```

### mosquitto.confファイルの先頭に，以下の3行を追加
```bash
sudo -e /etc/mosquitto/mosquitto.conf
```

```conf
listener  1883 # MQTTを1883番ポートで受信する
listener 15675 # WebSocketを15675番ポートで受信する
protocol websockets
```

### 自動起動の設定とサービスの再起動
```bash
sudo systemctl enable mosquitto.service
sudo sudo systemctl restart mosquitto
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
# install paho.mqtt.c
cd ~
sudo apt-get install fakeroot devscripts dh-make lsb-release
sudo apt-get install libssl-dev
sudo apt-get install doxygen graphviz
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.12
mkdir build && cd build
cmake ..
make -j4
sudo make install
sudo ldconfig
# install paho.mqtt.cpp
cd ~
git clone https://github.com/eclipse/paho.mqtt.cpp.git
cd paho.mqtt.cpp
mkdir build && cd build
cmake ..
make -j4
sudo make install
sudo ldconfig
```
