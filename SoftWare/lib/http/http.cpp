#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include "http.h"
#include "led.h"

static DNSServer dnsServer;
static ESP8266WebServer server(80);

static const String root = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Flowing Rainbow</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', sans-serif;
            background: #1a1a2e;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            color: white;
        }
        h1 {
            font-size: 2.5em;
            margin-bottom: 30px;
            background: linear-gradient(45deg, #ff006e, #fb5607, #ffbe0b, #8338ec);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .color-wheel {
            width: 300px;
            height: 300px;
            border-radius: 50%;
            position: relative;
            cursor: crosshair;
            margin-bottom: 30px;
            box-shadow: 0 0 30px rgba(0,0,0,0.5);
            background: radial-gradient(circle, white 0%, transparent 60%),
                        conic-gradient(from 0deg, 
                            hsl(0,100%,50%), 
                            hsl(60,100%,50%), 
                            hsl(120,100%,50%), 
                            hsl(180,100%,50%), 
                            hsl(240,100%,50%), 
                            hsl(300,100%,50%), 
                            hsl(360,100%,50%)
                        );
        }
        .picker {
            position: absolute;
            width: 24px;
            height: 24px;
            border: 3px solid white;
            border-radius: 50%;
            box-shadow: 0 2px 10px rgba(0,0,0,0.5);
            pointer-events: none;
            transform: translate(-50%, -50%);
            background: #ffffff;
            left: 50%;
            top: 50%;
            z-index: 10;
        }
        .color-display {
            font-size: 1.5em;
            font-family: monospace;
            margin-bottom: 30px;
            padding: 10px 20px;
            background: rgba(255,255,255,0.1);
            border-radius: 10px;
        }
        .buttons {
            display: flex;
            gap: 15px;
        }
        button {
            padding: 12px 30px;
            border: none;
            border-radius: 25px;
            cursor: pointer;
            font-size: 1em;
            color: white;
            transition: transform 0.2s;
        }
        button:hover {
            transform: scale(1.05);
        }
        .btn-effect1 { background: #ff006e; }
        .btn-effect2 { background: #3a86ff; }
        .btn-close { background: #fb5607; }
    </style>
</head>
<body>
    <h1>Flowing Rainbow</h1>
    <div class="color-wheel" id="colorWheel">
        <div class="picker" id="picker"></div>
    </div>
    <div class="color-display" id="colorDisplay">#FFFFFF</div>
    <div class="buttons">
        <button class="btn-effect1" onclick="sendCommand('effect1')">渐变</button>
        <button class="btn-effect2" onclick="sendCommand('effect2')">闪烁</button>
        <button class="btn-close" onclick="sendCommand('close')">关闭</button>
    </div>
    <script>
        const colorWheel = document.getElementById('colorWheel');
        const picker = document.getElementById('picker');
        const colorDisplay = document.getElementById('colorDisplay');

        function getColorAtPoint(x, y) {
            const rect = colorWheel.getBoundingClientRect();
            const centerX = rect.width / 2;
            const centerY = rect.height / 2;
            const dx = x - rect.left - centerX;
            const dy = y - rect.top - centerY;
            const distance = Math.sqrt(dx*dx + dy*dy) / (rect.width / 2);
            const clampedDist = Math.min(distance, 1);
            
            let angle = Math.atan2(dy, dx) * 180 / Math.PI;
            const hue = (angle + 90 + 360) % 360;
            const saturation = clampedDist * 100;
            const lightness = 50 + (1 - clampedDist) * 50;
            
            return hslToHex(hue, saturation, lightness);
        }

        function hslToHex(h, s, l) {
            l /= 100;
            const a = s * Math.min(l, 1 - l) / 100;
            const f = n => {
                const k = (n + h / 30) % 12;
                const color = l - a * Math.max(Math.min(k - 3, 9 - k, 1), -1);
                return Math.round(255 * color).toString(16).padStart(2, '0');
            };
            return `#${f(0)}${f(8)}${f(4)}`.toUpperCase();
        }

        function handleClick(e) {
            e.preventDefault();
            e.stopPropagation();
            
            const clientX = e.clientX || (e.touches && e.touches[0].clientX);
            const clientY = e.clientY || (e.touches && e.touches[0].clientY);
            if (!clientX || !clientY) return;
            
            const hex = getColorAtPoint(clientX, clientY);
            const rect = colorWheel.getBoundingClientRect();
            
            picker.style.left = (clientX - rect.left) + 'px';
            picker.style.top = (clientY - rect.top) + 'px';
            picker.style.backgroundColor = hex;
            colorDisplay.textContent = hex;
            colorDisplay.style.color = hex;
            
            fetch('/setcolor?color=' + encodeURIComponent(hex)).catch(() => {});
        }

        let isDragging = false;
        
        colorWheel.addEventListener('mousedown', handleClick, true);
        colorWheel.addEventListener('touchstart', handleClick, {passive: false, capture: true});
        colorWheel.addEventListener('mousedown', () => isDragging = true);
        colorWheel.addEventListener('touchstart', () => isDragging = true, {passive: false});
        
        document.addEventListener('mousemove', e => { if (isDragging) handleClick(e); });
        document.addEventListener('touchmove', e => { if (isDragging) handleClick(e); }, {passive: false});
        document.addEventListener('mouseup', () => isDragging = false);
        document.addEventListener('touchend', () => isDragging = false);

        function sendCommand(cmd) {
            fetch('/command?cmd=' + cmd).catch(() => {});
        }
    </script>
</body>
</html>
)rawliteral";

void handleSetColor(void) {
  if (server.hasArg("color")) {
    String colorStr = server.arg("color");  
    Serial.print("收到：");
    Serial.println(colorStr);
    
    if (colorStr.length() == 7 && colorStr[0] == '#') {
      long color = strtol(colorStr.substring(1).c_str(), NULL, 16);

      uint32_t color_32 = color & 0x00FFFFFF;
      led_set_color(color_32);
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleCommand() 
{
    if (server.hasArg("cmd")) 
    {
        String cmd = server.arg("cmd");
        
        if (cmd == "effect1") 
        {
            led_gradual_Mode();
        }
        else if (cmd == "effect2") 
        {
            led_flicker_Mode();
        }
        else if (cmd == "close") 
        {
            led_off();
        }
    }
    server.send(200, "text/plain", "OK");
}

void http_server_init(void)
{
    IPAddress myIP = WiFi.softAPIP();
    // dnsServer.start(53, "*", myIP);
    
    server.onNotFound([myIP]() {
        String host = server.hostHeader();
        String uri = server.uri();
        
        server.sendHeader("Location", "http://" + myIP.toString() + "/", true);
        server.send(302, "text/plain", "");
    });
    
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", root);
    });
    server.on("/setcolor", handleSetColor); 
    server.on("/command", handleCommand);
    
    // server.begin();
    
}

void http_lowpower_on(void)
{
    IPAddress myIP = WiFi.softAPIP();
    dnsServer.start(53, "*", myIP);

    server.begin();
}

void http_lowpower_off(void)
{
    dnsServer.stop();
    server.close();
}

void http_server_loop(void) 
{
    dnsServer.processNextRequest(); 
    server.handleClient();
}
