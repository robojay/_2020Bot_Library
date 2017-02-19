#ifndef Bot_Remote_h
#define Bot_Remote_h

const int32_t Remote_no_code = 0x00000000;
const int32_t Remote_undefined = 0x00000000;

#ifndef RemoteType
    #define RemoteType  BotRemote
#endif

#if RemoteType == BotRemote
    // Standard 2020 Bot Remote
    const int32_t Remote_ch_minus = Remote_undefined + 1;
    const int32_t Remote_ch = Remote_undefined + 2;
    const int32_t Remote_ch_plus = Remote_undefined + 3;
    const int32_t Remote_prev = Remote_undefined + 4;
    const int32_t Remote_next = Remote_undefined + 5;
    const int32_t Remote_play = Remote_undefined + 6;
    const int32_t Remote_vol_minus = Remote_undefined + 7;
    const int32_t Remote_vol_plus = Remote_undefined + 8;
    const int32_t Remote_eq = Remote_undefined + 9;
    const int32_t Remote_0 = 0x00FF9867;
    const int32_t Remote_100 = Remote_undefined + 10;
    const int32_t Remote_200 = Remote_undefined + 11;
    const int32_t Remote_1 = 0x00FFA25D;
    const int32_t Remote_2 = 0x00FF629D;
    const int32_t Remote_3 = 0x00FFE21D;
    const int32_t Remote_4 = 0x00FF22DD;
    const int32_t Remote_5 = 0x00FF02FD;
    const int32_t Remote_6 = 0x00FFC23D;
    const int32_t Remote_7 = 0x00FFE01F;
    const int32_t Remote_8 = 0x00FFA857;
    const int32_t Remote_9 = 0x00FF906F;
    const int32_t Remote_asterisk = 0x00FF6897;
    const int32_t Remote_hash = 0x00FFB04F;
    const int32_t Remote_ok = 0x00FF38C7;
    const int32_t Remote_up_arrow = 0x00FF18E7;
    const int32_t Remote_down_arrow = 0x00FF4AB5;
    const int32_t Remote_left_arrow = 0x00FF10EF;
    const int32_t Remote_right_arrow = 0x00FF5AA5;

#elif RemoteType == VelikkaRemote
    // Velikka Remote
    const int32_t Remote_ch_minus = 0x00FFA25D;
    const int32_t Remote_ch = 0x00FF629D;
    const int32_t Remote_ch_plus = 0x00FFE21D;
    const int32_t Remote_prev = 0x00FF22DD;
    const int32_t Remote_next = 0x00FF02FD;
    const int32_t Remote_play = 0x00FFC23D;
    const int32_t Remote_vol_minus = 0x00FFE01F;
    const int32_t Remote_vol_plus = 0x00FFA857;
    const int32_t Remote_eq = 0x00FF906F;
    const int32_t Remote_0 = 0x00FF6897;
    const int32_t Remote_100 = 0x00FF9867;
    const int32_t Remote_200 = 0x00FFB04F;
    const int32_t Remote_1 = 0x00FF30CF;
    const int32_t Remote_2 = 0x00FF18E7;
    const int32_t Remote_3 = 0x00FF7A85;
    const int32_t Remote_4 = 0x00FF10EF;
    const int32_t Remote_5 = 0x00FF38C7;
    const int32_t Remote_6 = 0x00FF5AA5;
    const int32_t Remote_7 = 0x00FF42BD;
    const int32_t Remote_8 = 0x00FF4AB5;
    const int32_t Remote_9 = 0x00FF52AD;
    const int32_t Remote_asterisk = Remote_undefined + 1;
    const int32_t Remote_hash = Remote_undefined + 2;
    const int32_t Remote_ok = Remote_undefined + 3;
    const int32_t Remote_up_arrow = Remote_undefined + 4;
    const int32_t Remote_down_arrow = Remote_undefined + 5;
    const int32_t Remote_left_arrow = Remote_undefined + 6;
    const int32_t Remote_right_arrow = Remote_undefined + 7;

#endif

#endif
