import pty
import tty
import os
import random
import time

class Command:
    NOP = 0
    ACK = 1
    CAPTURE = 2

class PacketData:
    def __init__(self, command, length, data, fcs):
        self.command = command
        self.length  = length
        self.data    = data
        self.fcs     = fcs

    def copy(self):
        return PacketData(self.command, self.length, self.data, self.fcs)

    def clear(self):
        self.command = 0x0
        self.length = 0x0
        self.data = []
        self.fcs = 0x0

    def __repr__(self):
        return f"""
    Packet data:
    -----------------------------
    Command: {self.command}
    Length: {self.length}
    Data: {self.data}
    FCS: {self.fcs}
"""

    def pack(self):
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

class PacketParser:
    def __init__(self):
        self.currentPacket = PacketData(0, 0, [], 0)
        self.bytesread = 0

    def parse(self, bytedata):
        if bytedata == None:
            return []

        output = []

        for byte in bytedata:
            if self.bytesread == 0:
                self.currentPacket.command  = byte

            elif self.bytesread == 1:
                self.currentPacket.length   = byte
                
            elif self.bytesread == 2:
                self.currentPacket.length  |= (byte << 8)
                self.currentPacket.data     = [0] * self.currentPacket.length

            elif (self.bytesread - 3) < self.currentPacket.length:
                self.currentPacket.data[self.bytesread - 3] = byte

            elif self.bytesread == (3 + self.currentPacket.length + 0):
                self.currentPacket.fcs = byte
                
            elif self.bytesread == (3 + self.currentPacket.length + 1):
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

def send_ack(fd):
    packet = PacketData(Command.ACK, 0, [], 0)
    payload = packet.pack()
    print("To LogicAnalyzer:", payload)
    packet.send(fd)

def send_capture(fd, size):
    packet = PacketData(Command.CAPTURE, size, random.randbytes(size), 0)
    payload = packet.pack()
    print("To LogicAnalyzer:", payload)
    packet.send(fd)
    
        
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

    send_ack(master)
    send_capture(master, 32)
