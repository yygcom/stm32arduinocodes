
#include <Arduino.h>
#include <USBComposite.h>

USBHID HID;

const uint8_t reportDescription[] = {
   HID_CONSUMER_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR()
};

HIDConsumer Consumer(HID);
HIDKeyboard Keyboard(HID);

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

#include <IRremote.hpp>


// 辅助函数：将 uint32_t 转换为十六进制字符串
String uint32ToHexString(uint32_t num) {
    char hexString[11]; // 10 个字符 + 1 个空终止符
    snprintf(hexString, sizeof(hexString), "0x%08X", num);
    return String(hexString);
}


void setup() {
    HID.begin(reportDescription, sizeof(reportDescription));
    Serial1.begin(115200);

    IrReceiver.begin(PB9, ENABLE_LED_FEEDBACK);

    
}

void loop() {
  if (IrReceiver.decode()) {
        // 获取解码后的协议和命令
        auto protocol = IrReceiver.decodedIRData.protocol;
        uint32_t command = IrReceiver.decodedIRData.decodedRawData;

        // 打印解码信息到串口
        Serial1.print(F("Decoded protocol: "));
        Serial1.print(getProtocolString(IrReceiver.decodedIRData.protocol));
        Serial1.print(F(", decoded raw data: "));
#if (__INT_WIDTH__ < 32)
        Serial1.print(command, HEX);
#else
        PrintULL::print(&Serial, command, HEX);
#endif
        Serial1.print(F(", decoded address: "));
        Serial1.print(IrReceiver.decodedIRData.address, HEX);
        Serial1.print(F(", decoded command: "));
        Serial1.println(IrReceiver.decodedIRData.command, HEX);

        // 根据接收到的 NEC 命令执行操作
        if (protocol == NEC) {
            // 将命令值转换为十六进制字符串
            /*
            String hexString = uint32ToHexString(command);

            // 通过键盘输入发送十六进制字符串
            for (size_t i = 2; i < hexString.length(); ++i) {
                char c = hexString.charAt(i);
                Keyboard.press(c);
                delay(50); // 确保按键按下的时间足够长
                Keyboard.release(c);
                delay(50); // 确保按键释放的时间足够长
            }
            */
            switch (command) {
                case 0xfa05fb04: // 替换为你接收的增加音量的 NEC 命令值
                    Consumer.press(HIDConsumer::VOLUME_UP);
                    delay(50); // 确保按键按下的时间足够长
                    Consumer.release();
                    Serial.println("Volume Up");
                    break;
                case 0xfe01fb04: // 替换为你接收的减小音量的 NEC 命令值
                    Consumer.press(HIDConsumer::VOLUME_DOWN);
                    delay(50); // 确保按键按下的时间足够长
                    Consumer.release();
                    Serial.println("Volume Down");
                    break;
                default:
                    // 其他命令处理
                    break;
            }

            Serial1.print("Hex String sent: ");
            //Serial.println(hexString);
        }

        // 恢复红外接收
        IrReceiver.resume();
    }
}
