/***** The Things Network Keys *****/

// Network Session Key (MSB)                                                                                      // These 3 keys change from 
uint8_t NwkSkey[16] = { 0x2D,0x43,0xB1,0x33,0x0A,0x67,0x4A,0x59,0x5F,0x21,0x94,0x9E,0x84,0x60,0xF5,0x6C};        // device to device. After  
                                                                                                                  // adding a new device to 
// Application Session Key (MSB)                                                                                  // Chirpstack, a set of keys  
uint8_t AppSkey[16] = { 0xBB,0xC5,0xD3,0x3C,0xF0,0x8C,0xC9,0xCD,0xBF,0x0C,0xD1,0x88,0x08,0x87,0x4B,0x66};        // is assigned. Copy them 
                                                                                                                  // and change this code 
// Device Address (MSB)                                                                                           // accordingly.
uint8_t DevAddr[4] =  { 0x01,0x38,0x25,0x88} ;  
