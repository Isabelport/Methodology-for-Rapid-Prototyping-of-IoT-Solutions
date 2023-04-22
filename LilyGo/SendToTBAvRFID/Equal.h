// We need this header file to use FLASH as storage with PROGMEM directive:

// Icon width and height
const uint16_t equalWidth = 32;
const uint16_t equalHeight = 32;

const unsigned short equal[1024] PROGMEM={
0x0020, 0x0021, 0x0001, 0x0000, 0x0020, 0x0040, 0x0000, 0x0042, 0x0003, 0x0066, 0x016A, 0x028F, 0x03F3, 0x0D38, 0x161B, 0x0E5B,   // 0x0010 (16) pixels
0x063A, 0x0E1A, 0x0D37, 0x03D2, 0x026D, 0x01A9, 0x00E6, 0x0043, 0x0042, 0x0000, 0x0800, 0x0800, 0x0000, 0x0000, 0x0000, 0x0021,   // 0x0020 (32) pixels
0x0020, 0x0000, 0x0021, 0x0021, 0x0000, 0x0061, 0x0020, 0x0083, 0x0189, 0x2331, 0x3CB7, 0x3D9A, 0x363C, 0x1E1B, 0x05DA, 0x0E1B,   // 0x0030 (48) pixels
0x05FA, 0x0E1A, 0x1DFA, 0x35FA, 0x3DBA, 0x3CF6, 0x1B50, 0x01C9, 0x0083, 0x0021, 0x0041, 0x0000, 0x0820, 0x0840, 0x0000, 0x0021,   // 0x0040 (64) pixels
0x0000, 0x0020, 0x0021, 0x0000, 0x0001, 0x0082, 0x0145, 0x136E, 0x4598, 0x35BA, 0x2DDB, 0x1DDB, 0x0DFA, 0x0E1B, 0x0E1C, 0x0DFC,   // 0x0050 (80) pixels
0x163C, 0x163C, 0x0E1B, 0x0DFA, 0x15FA, 0x261A, 0x2DD9, 0x3D77, 0x132E, 0x0126, 0x0022, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0040, 0x0000, 0x0021, 0x0063, 0x0187, 0x2CB3, 0x2DB8, 0x261A, 0x15FA, 0x163B, 0x0E3B, 0x061A, 0x063B, 0x163C, 0x0DDC,   // 0x0070 (112) pixels
0x0DFB, 0x0DFB, 0x061B, 0x061B, 0x063B, 0x0E5B, 0x163A, 0x25F9, 0x35B8, 0x3473, 0x01A8, 0x0043, 0x0042, 0x0020, 0x0020, 0x0040,   // 0x0080 (128) pixels
0x0000, 0x0020, 0x0021, 0x0063, 0x0187, 0x4536, 0x2DD9, 0x1E3B, 0x05FA, 0x065C, 0x063B, 0x063A, 0x0E5B, 0x0E3B, 0x0DFB, 0x163D,   // 0x0090 (144) pixels
0x163C, 0x161C, 0x061B, 0x063C, 0x065C, 0x065C, 0x063B, 0x0DFA, 0x1DFA, 0x35D9, 0x3CF6, 0x0188, 0x0084, 0x0001, 0x0020, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0x0041, 0x0042, 0x0188, 0x3D16, 0x2DD9, 0x1E3B, 0x061B, 0x065D, 0x065C, 0x063B, 0x063A, 0x0E3A, 0x0E3B, 0x061B, 0x05FC,   // 0x00B0 (176) pixels
0x0DFB, 0x0E1B, 0x0E3C, 0x063C, 0x061B, 0x061B, 0x061C, 0x0E3C, 0x0E1B, 0x1DFB, 0x35FB, 0x3D17, 0x01C9, 0x0083, 0x0041, 0x0020,   // 0x00C0 (192) pixels
0x0042, 0x0022, 0x0106, 0x34D5, 0x25D9, 0x163B, 0x05FB, 0x063D, 0x061C, 0x05DC, 0x063C, 0x0E3C, 0x0DFA, 0x0DFB, 0x0E5D, 0x065D,   // 0x00D0 (208) pixels
0x061C, 0x061B, 0x05FB, 0x061C, 0x0E1C, 0x0E3D, 0x0E1D, 0x0E1C, 0x061C, 0x05FB, 0x15DB, 0x2DDA, 0x34F6, 0x0106, 0x0041, 0x0041,   // 0x00E0 (224) pixels
0x0001, 0x0084, 0x0B2F, 0x2DB9, 0x163B, 0x061B, 0x0E3C, 0x05FB, 0x0DFC, 0x161C, 0x0DFB, 0x15FB, 0x1DFB, 0x15FB, 0x05FB, 0x061C,   // 0x00F0 (240) pixels
0x0E1B, 0x0DFC, 0x0E1C, 0x15FC, 0x15FC, 0x15FC, 0x15DB, 0x0DBB, 0x0DFC, 0x0E3D, 0x05FB, 0x1E1B, 0x35B9, 0x134F, 0x0083, 0x0022,   // 0x0100 (256) pixels
0x0002, 0x0167, 0x3D98, 0x15FA, 0x165C, 0x0DFB, 0x1DFB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x1DFC,   // 0x0110 (272) pixels
0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25FB, 0x25DB, 0x25DB, 0x2DDB, 0x2E1C, 0x15DB, 0x05DB, 0x163B, 0x25FA, 0x3D77, 0x01A8, 0x0022,   // 0x0120 (288) pixels
0x0044, 0x232F, 0x363B, 0x0E1B, 0x05FB, 0x15FB, 0x2558, 0x2D16, 0x2D16, 0x2D16, 0x2D17, 0x2D17, 0x2D17, 0x2D17, 0x2D17, 0x2D17,   // 0x0130 (304) pixels
0x2D17, 0x2D17, 0x2D17, 0x2D17, 0x2D17, 0x2D16, 0x34F6, 0x34F7, 0x2CF7, 0x2D79, 0x161B, 0x0E1B, 0x0DD9, 0x35F9, 0x1B6F, 0x00A5,   // 0x0140 (320) pixels
0x0188, 0x3CF6, 0x1E1B, 0x063B, 0x0DFB, 0x25B9, 0x02EE, 0x0106, 0x0146, 0x0126, 0x0126, 0x0127, 0x0126, 0x0126, 0x0126, 0x0126,   // 0x0150 (336) pixels
0x0126, 0x0126, 0x0126, 0x0126, 0x0126, 0x0125, 0x0125, 0x0126, 0x0127, 0x0B30, 0x25B9, 0x161A, 0x05F9, 0x1E19, 0x3516, 0x01A8,   // 0x0160 (352) pixels
0x02CD, 0x3DD9, 0x0DFA, 0x063C, 0x161B, 0x2D99, 0x022A, 0x0042, 0x0021, 0x0021, 0x0022, 0x0022, 0x0022, 0x0021, 0x0020, 0x0020,   // 0x0170 (368) pixels
0x0020, 0x0020, 0x0021, 0x0021, 0x0021, 0x0041, 0x0020, 0x0041, 0x0043, 0x024C, 0x2D98, 0x161A, 0x063A, 0x163A, 0x35D9, 0x028C,   // 0x0180 (384) pixels
0x0433, 0x25FA, 0x05FB, 0x063C, 0x161B, 0x2578, 0x022A, 0x0042, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0020, 0x0040,   // 0x0190 (400) pixels
0x0040, 0x0020, 0x0020, 0x0021, 0x0041, 0x0041, 0x0041, 0x0062, 0x0083, 0x026C, 0x35B9, 0x15FA, 0x065B, 0x063A, 0x1DFA, 0x0412,   // 0x01A0 (416) pixels
0x0D78, 0x15FB, 0x061C, 0x061C, 0x161B, 0x2578, 0x028B, 0x00A3, 0x0083, 0x0063, 0x0083, 0x0083, 0x0063, 0x0083, 0x0082, 0x0082,   // 0x01B0 (432) pixels
0x0081, 0x0081, 0x0082, 0x0083, 0x0083, 0x00A3, 0x00A2, 0x00C3, 0x00C4, 0x02CD, 0x2DBA, 0x15DB, 0x063C, 0x063C, 0x15FB, 0x0D79,   // 0x01C0 (448) pixels
0x0DDA, 0x0E1B, 0x0E1D, 0x0E1D, 0x161C, 0x1DBA, 0x14B4, 0x0BF1, 0x1BF1, 0x1BF1, 0x1BF2, 0x1BF2, 0x1BF2, 0x1BF2, 0x1BF2, 0x1BD2,   // 0x01D0 (464) pixels
0x23F1, 0x1BD1, 0x1BD2, 0x1BF2, 0x1412, 0x1412, 0x1411, 0x1C11, 0x13F1, 0x1CD6, 0x25DB, 0x0DDC, 0x063D, 0x0E5D, 0x0DFB, 0x15FB,   // 0x01E0 (480) pixels
0x0DFC, 0x0E1C, 0x05FD, 0x0E1D, 0x1E1C, 0x1DDB, 0x25FA, 0x361A, 0x35F9, 0x35DA, 0x35BA, 0x35DA, 0x35DA, 0x35DA, 0x3DBA, 0x3DBA,   // 0x01F0 (496) pixels
0x3DB9, 0x3DB9, 0x35BA, 0x35DA, 0x2DFA, 0x2DD9, 0x35D9, 0x35D8, 0x35D9, 0x2DFA, 0x1DFB, 0x0E1C, 0x05FC, 0x061D, 0x0DBB, 0x1E1C,   // 0x0200 (512) pixels
0x0DFB, 0x0E1C, 0x061D, 0x05FC, 0x15BB, 0x1DDA, 0x2E1B, 0x2DFA, 0x2DFA, 0x2DFA, 0x35DA, 0x35DA, 0x35D9, 0x35DA, 0x35BB, 0x3DBB,   // 0x0210 (528) pixels
0x3DBA, 0x3DD9, 0x35F9, 0x35FA, 0x2E19, 0x35F9, 0x35D9, 0x35D9, 0x35FA, 0x25FB, 0x15DA, 0x0E1C, 0x0E1C, 0x0E1C, 0x161D, 0x15DC,   // 0x0220 (544) pixels
0x15FB, 0x0DFC, 0x061C, 0x0E3D, 0x161C, 0x25DA, 0x14B5, 0x0BF2, 0x0BF2, 0x0BF2, 0x0BD1, 0x13D1, 0x13D0, 0x13D1, 0x13D2, 0x13B2,   // 0x0230 (560) pixels
0x13B1, 0x13D1, 0x0BD0, 0x0BF0, 0x0BF0, 0x0BD0, 0x13B1, 0x13D1, 0x0BF2, 0x14D6, 0x1DDA, 0x161B, 0x061B, 0x05FB, 0x1E1C, 0x1DDC,   // 0x0240 (576) pixels
0x1538, 0x1DFB, 0x061B, 0x063C, 0x0E3B, 0x25B8, 0x028C, 0x0063, 0x0084, 0x0083, 0x0083, 0x00A3, 0x0082, 0x0082, 0x0082, 0x0082,   // 0x0250 (592) pixels
0x0082, 0x00A2, 0x00A2, 0x0083, 0x0082, 0x0083, 0x0083, 0x00A4, 0x0064, 0x02CE, 0x2DBA, 0x161B, 0x05FB, 0x061B, 0x1E1C, 0x0518,   // 0x0260 (608) pixels
0x03D1, 0x2DFA, 0x063B, 0x063B, 0x061A, 0x1D98, 0x022A, 0x0042, 0x0001, 0x0001, 0x0021, 0x0021, 0x0000, 0x0000, 0x0020, 0x0020,   // 0x0270 (624) pixels
0x0020, 0x0020, 0x0021, 0x0021, 0x0001, 0x0001, 0x0001, 0x0043, 0x0044, 0x0A8D, 0x35BA, 0x15FB, 0x061B, 0x0E3B, 0x2E1C, 0x03B2,   // 0x0280 (640) pixels
0x02CC, 0x3DB8, 0x0E1A, 0x063B, 0x165C, 0x1D78, 0x020A, 0x0042, 0x0021, 0x0001, 0x0021, 0x0022, 0x0021, 0x0021, 0x0020, 0x0040,   // 0x0290 (656) pixels
0x0020, 0x0020, 0x0021, 0x0022, 0x0022, 0x0021, 0x0021, 0x0022, 0x0084, 0x026C, 0x2D99, 0x15FB, 0x0E1B, 0x0DDA, 0x3DDA, 0x02CE,   // 0x02A0 (672) pixels
0x01C8, 0x3D15, 0x263A, 0x05FA, 0x163C, 0x25B9, 0x030E, 0x0125, 0x0146, 0x0146, 0x0147, 0x0147, 0x0147, 0x0147, 0x0166, 0x0165,   // 0x02B0 (688) pixels
0x0165, 0x0166, 0x0167, 0x0167, 0x0147, 0x0147, 0x0146, 0x0147, 0x0148, 0x0330, 0x25BA, 0x15FB, 0x0DFA, 0x25D9, 0x3D16, 0x01A9,   // 0x02C0 (704) pixels
0x0083, 0x1B4E, 0x363B, 0x15DA, 0x0DBB, 0x1E1C, 0x2D78, 0x24D5, 0x2CF5, 0x34F5, 0x2CD6, 0x2CD7, 0x2CF7, 0x2CF7, 0x2CF6, 0x2D16,   // 0x02D0 (720) pixels
0x2D15, 0x2D16, 0x2CF6, 0x2CF6, 0x2CF6, 0x2CF6, 0x34D5, 0x34D6, 0x24D6, 0x2599, 0x1E1B, 0x0DFA, 0x15FA, 0x3E3A, 0x236F, 0x0063,   // 0x02E0 (736) pixels
0x0062, 0x0187, 0x3D58, 0x2E1B, 0x15FC, 0x15FC, 0x1DDB, 0x25FB, 0x25FA, 0x25FA, 0x25DB, 0x25DC, 0x25FC, 0x25FC, 0x25FB, 0x25FA,   // 0x02F0 (752) pixels
0x261B, 0x261A, 0x25FB, 0x25FB, 0x25FA, 0x2DFA, 0x2DDA, 0x2DDA, 0x25FB, 0x15FB, 0x05DA, 0x163B, 0x25D9, 0x4597, 0x01A8, 0x0042,   // 0x0300 (768) pixels
0x0062, 0x00A4, 0x134F, 0x35B9, 0x15FB, 0x0E3C, 0x0DFC, 0x15FB, 0x15FB, 0x15FB, 0x15DB, 0x0DDC, 0x15FC, 0x15FC, 0x15FB, 0x0DFA,   // 0x0310 (784) pixels
0x0E1A, 0x0DFA, 0x163B, 0x15FB, 0x1E1B, 0x15DA, 0x1DDB, 0x15FC, 0x0DDB, 0x0E3C, 0x061B, 0x161A, 0x35D8, 0x134E, 0x00C4, 0x0021,   // 0x0320 (800) pixels
0x0021, 0x0042, 0x00C5, 0x3CF6, 0x25DA, 0x161B, 0x0DFB, 0x0E3D, 0x0DFB, 0x0E1C, 0x0E1C, 0x0E1C, 0x0DFC, 0x0E1C, 0x0E1C, 0x0E1B,   // 0x0330 (816) pixels
0x0E3C, 0x0E1B, 0x0E1B, 0x05FB, 0x0E1B, 0x0DFB, 0x0DFC, 0x05DB, 0x0E5D, 0x0E1C, 0x15FB, 0x2DD9, 0x34D5, 0x0105, 0x0021, 0x0001,   // 0x0340 (832) pixels
0x0000, 0x0021, 0x0063, 0x01A8, 0x3516, 0x361B, 0x15DA, 0x0E1C, 0x0E3C, 0x061B, 0x061B, 0x061B, 0x0E3C, 0x0E3C, 0x0E1C, 0x0E1B,   // 0x0350 (848) pixels
0x0DFB, 0x0E1C, 0x0E1C, 0x0E3C, 0x063C, 0x063C, 0x0E5D, 0x165D, 0x0DFC, 0x1DDB, 0x35BA, 0x3D37, 0x01A8, 0x00A4, 0x0042, 0x0821,   // 0x0360 (864) pixels
0x0000, 0x0021, 0x0041, 0x00A3, 0x01A8, 0x3D37, 0x2DDA, 0x1E3B, 0x0E1A, 0x063B, 0x0E3B, 0x0E3C, 0x0E3C, 0x061C, 0x0E1C, 0x0E3C,   // 0x0370 (880) pixels
0x0E1C, 0x161C, 0x05FC, 0x061C, 0x061C, 0x065C, 0x061B, 0x05DA, 0x25FB, 0x35BA, 0x3CD6, 0x01A9, 0x0084, 0x0021, 0x0021, 0x0000,   // 0x0380 (896) pixels
0x0000, 0x0821, 0x0000, 0x0040, 0x0063, 0x0187, 0x2CD5, 0x25B8, 0x1DF9, 0x1619, 0x161A, 0x0E1A, 0x061B, 0x061C, 0x061C, 0x05FC,   // 0x0390 (912) pixels
0x0DFC, 0x0E1D, 0x0DFD, 0x0E1D, 0x061C, 0x165C, 0x161A, 0x1DD9, 0x3DD9, 0x3474, 0x01EA, 0x0064, 0x0043, 0x0001, 0x0000, 0x0000,   // 0x03A0 (928) pixels
0x0001, 0x0000, 0x0820, 0x0000, 0x0020, 0x0083, 0x0126, 0x0B8F, 0x3597, 0x2DD8, 0x25F9, 0x1DDA, 0x0DFB, 0x0E1C, 0x0E5D, 0x0E5C,   // 0x03B0 (944) pixels
0x061B, 0x0E1C, 0x0DDC, 0x15BB, 0x15BB, 0x25FB, 0x2DD9, 0x3D77, 0x132D, 0x0105, 0x0043, 0x0001, 0x0001, 0x0000, 0x0820, 0x0000,   // 0x03C0 (960) pixels
0x0001, 0x0001, 0x0000, 0x0040, 0x0000, 0x0000, 0x0042, 0x0043, 0x01E9, 0x1B8F, 0x3536, 0x35FA, 0x25FB, 0x1E1C, 0x0E1B, 0x061A,   // 0x03D0 (976) pixels
0x063A, 0x0E1A, 0x25FB, 0x35DC, 0x45BC, 0x3CF7, 0x1B8F, 0x01E8, 0x0082, 0x0020, 0x0882, 0x0000, 0x0021, 0x0041, 0x0000, 0x0020,   // 0x03E0 (992) pixels
0x0002, 0x0001, 0x0020, 0x0020, 0x0040, 0x0000, 0x0001, 0x0022, 0x0022, 0x00C5, 0x01C9, 0x02AD, 0x03F3, 0x0D58, 0x0E3B, 0x065A,   // 0x03F0 (1008) pixels
0x067A, 0x0E19, 0x1518, 0x0373, 0x026F, 0x0169, 0x00E5, 0x0061, 0x0040, 0x0000, 0x0000, 0x0020, 0x0000, 0x0040, 0x0040, 0x0020,   // 0x0400 (1024) pixels
};