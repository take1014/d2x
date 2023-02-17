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