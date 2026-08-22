import pty
import tty
import os
import random
import time
import numpy

class Command:
    NOP = 0
    ACK = 1
    CAPTURE = 2

def modbusCRC16(crc, byte, init):
    if(init): crc = numpy.uint16(0xFFFF);
        
    crc ^= (byte & 0xFF);
    
    for _ in range(8):
        if(crc & 0x0001):
            crc = (crc >> 1) ^ 0xA001;
        else:
            crc = (crc >> 1);

    return crc;
   
class PacketData:
    def __init__(self, command, length, data, transactionId=0, headerChecksum=0, version=0, flags=0, magic=0):
        self.version = version
        self.command = command
        self.length  = length
        self.transactionId = transactionId
        self.flags = flags
        self.headerChecksum = headerChecksum
        self.data    = data if data is not None else []
        self.magic   = magic
        self.fcs     = self.calculate_fcs()

    def copy(self):
        return PacketData(self.command, self.length, self.data, self.transactionId, self.headerChecksum, self.version, self.flags, self.magic)

    def clear(self):
        self.magic = 0
        self.version = 0
        self.command = 0x0
        self.length = 0x0
        self.transactionId = 0
        self.flags = 0
        self.headerChecksum = 0
        self.data = []
        self.fcs = 0x0

    def __repr__(self):
        return f"""
    Packet data:
    -----------------------------
    Magic: {hex(self.magic)}
    Version: {self.version}
    Command: {self.command}
    Length: {self.length}
    Transaction ID: {hex(self.transactionId)}
    Flags: {hex(self.flags)}
    Header Checksum: {hex(self.headerChecksum)}
    Data: {self.data}
    FCS: {hex(self.fcs)}
"""

    def calculate_fcs(self):
        fcs = numpy.uint16(0xFFFF);

        fcs = modbusCRC16(fcs, self.command,              True)
        fcs = modbusCRC16(fcs, self.length        & 0xFF, False)
        fcs = modbusCRC16(fcs, (self.length >> 8) & 0xFF, False)

        for byte in self.data:
            fcs = modbusCRC16(fcs, byte, False)

        return fcs

    def pack(self):
        self.fcs = self.calculate_fcs()
        
        bytedata = bytearray(self.length + 5)
        bytedata[0] = self.command       & 0xFF
        bytedata[1] = (self.length)      & 0xFF
        bytedata[2] = (self.length >> 8) & 0xFF
        for i,byte in enumerate(self.data):
            bytedata[i+3] = byte
        bytedata[3 + self.length] = (self.fcs)      & 0xFF
        bytedata[4 + self.length] = (self.fcs >> 8) & 0xFF
        return bytes(bytedata)

    def send(self, fd):
        bytedata = self.pack()
        os.write(fd, bytedata)
        
    def send_sparse(self, fd, wait_time):
        bytedata = self.pack()
        for byte in bytedata:
            os.write(fd, bytes([byte]))
            time.sleep(wait_time)

class PacketParser:
    def __init__(self):
        self.currentPacket = PacketData(0, 0, None)
        self.bytesread = 0

    def parse(self, bytedata):
        if bytedata == None:
            return []

        output = []

        for byte in bytedata:
            if self.bytesread == 0:
                self.currentPacket.magic  = byte
                
            elif self.bytesread == 1:
                self.currentPacket.magic  |= (byte << 8)

            elif self.bytesread == 2:
                self.currentPacket.version  = byte

            elif self.bytesread == 3:
                self.currentPacket.command  = byte

            elif self.bytesread == 4:
                self.currentPacket.length   = byte
                
            elif self.bytesread == 5:
                self.currentPacket.length  |= (byte << 8)
                self.currentPacket.data     = [0] * self.currentPacket.length

            elif self.bytesread == 6:
                self.currentPacket.transactionId   = byte
                
            elif self.bytesread == 7:
                self.currentPacket.transactionId  |= (byte << 8)

            elif self.bytesread == 8:
                self.currentPacket.flags  = byte
                
            elif self.bytesread == 9:
                self.currentPacket.flags  |= (byte << 8)

            elif self.bytesread == 10:
                self.currentPacket.headerChecksum   = byte
                
            elif self.bytesread == 11:
                self.currentPacket.headerChecksum  |= (byte << 8)

            elif (self.bytesread > 11 and self.bytesread - 12) < self.currentPacket.length:
                self.currentPacket.data[self.bytesread - 12] = byte

            elif self.bytesread == (12 + self.currentPacket.length + 0):
                self.currentPacket.fcs = byte
                
            elif self.bytesread == (12 + self.currentPacket.length + 1):
                self.currentPacket.fcs |= (byte << 8)
                output.append(self.currentPacket.copy())
                self.currentPacket.clear()
                self.bytesread = 0
                continue

            self.bytesread += 1

        return output

    def reset(self):
        self.currentPacket.clear()
        self.bytesread = 0

            
master, slave = pty.openpty()

tty.setraw(slave)
parser = PacketParser()

print(os.ttyname(slave))
timeout = 3
lastRead = 0

def send_ack(fd, transactionId):
    packet = PacketData(Command.ACK, 2, [transactionId & 0xFF, (transactionId >> 8) & 0xFF])
    payload = packet.pack()
    print("To LogicAnalyzer:", payload)
    packet.send(fd)

def send_capture(fd, size):
    packet = PacketData(Command.CAPTURE, size, random.randbytes(size))
    payload = packet.pack()
    print("To LogicAnalyzer:", payload)
    packet.send(fd)
    
def send_capture_sparse(fd, size, wait_time):
    packet = PacketData(Command.CAPTURE, size, random.randbytes(size))
    payload = packet.pack()
    print("To LogicAnalyzer:", payload)
    packet.send_sparse(fd, wait_time)
    
        
while True:
    data = os.read(master, 4096)

    currentRead = time.monotonic()
    timeDelta = currentRead - lastRead
    if(timeDelta > timeout):
        print("Timed out, clearing packet data.")
        parser.reset()
    
    print("Raw bytes:", data)
    output = parser.parse(data)
    print("From LogicAnalyzer:", output, end="\n")
    print("Bytes read:", len(data))
    print("Time since last read:", timeDelta)
    lastRead = currentRead

    for packet in output:
        if packet.flags & 0x1: send_ack(master, packet.transactionId)

    send_capture(master, random.randint(3, 16))
