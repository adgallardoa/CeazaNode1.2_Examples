unsigned int calculateCRC16(unsigned char *buf, int len)
{
  unsigned int crc = 0xFFFF; // Initialize CRC16 to 0xFFFF

  for (int pos = 0; pos < len; pos++) {
    crc ^= (unsigned int)buf[pos]; // XOR byte into CRC

    for (int i = 8; i != 0; i--) { // Loop over each bit
      if ((crc & 0x0001) != 0) {   // If the LSB is set
        crc >>= 1;                // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else {                       // If the LSB is not set
        crc >>= 1;                // Shift right
      }
    }
  }

  return crc; // Return calculated CRC16
}



void generateModbusRTUQuery03(unsigned char* query, unsigned char slaveID, unsigned int startAddr, unsigned int numRegs)
{
  query[0] = slaveID;           // Slave ID
  query[1] = 0x03;              // Function code 03 (Read Holding Registers)
  query[2] = (startAddr >> 8);  // Starting address MSB
  query[3] = (startAddr & 0xFF);// Starting address LSB
  query[4] = (numRegs >> 8);    // Number of registers to read MSB
  query[5] = (numRegs & 0xFF);  // Number of registers to read LSB
  
  // Calculate CRC16 checksum and append it to query
  unsigned int crc = calculateCRC16(query, 6);
  query[6] = (crc & 0xFF);      // CRC LSB
  query[7] = ((crc >> 8) & 0xFF);// CRC MSB
}
