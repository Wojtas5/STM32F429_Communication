from scapy.all import *
import time


def sendUDSPacketandPrint(msg):
    # Create a message
    p = Ether(dst='02:00:00:00:00:00', type=2048) / IP(ihl=5, src=srcaddress, dst=destaddress) / msg

    # Send the message
    if __name__ == '__main__':
        sendp(p, iface=eth, count=1)
    else:
        sendp(p, iface=eth, count=1, verbose=False)

    # Wait for response
    r = sniff(filter='host 192.168.0.10', iface=eth, count=1)

    # Format the response to be visible as hex value
    packet = r[0]
    raw = packet.lastlayer()
    if __name__ == '__main__':
        hexdump(raw)

    return bytes(raw)


def fprintTimefromHex(secs):
    s = secs % 60
    m = secs/60
    h = m/60

    print('{:02d}:{:02d}:{:02d}'.format(int(h), int(m), int(s)))


def int_from_bytes(input):
    return int.from_bytes(input[3:5], byteorder='big', signed=True)


# Addresses of microcontroller and PC
destaddress = '192.168.0.10'
srcaddress  = '192.168.0.11'

# Name of network interface card
eth = 'Realtek PCIe GBE Family Controller'

# Service SIDs
TesterPresentSID = b'\x3E'
ECUResetSID = b'\x11'
ReadDataByIndentifierSID = b'\x22'
TimeFromStartupDID = b'\x01\x05'

# Other SIDs
DiagnosticSessionControlSID = b'\x10'
TesterPresentInvalidSID = b'\x3E\xFF\xFF'
ECUResetInvalidSID = b'\x11\x3E'
InvalidDID = b'\x01\x06'

# Response SIDs
TesterPresentRPSID = b'\x7E\x3E'
DiagnosticSessionControlRPSID = b'\x7E\x50'
ECUResetRPSID = b'\x51\x11'

TesterPresentInvalidRPSID = b'\x7F\x3E\x13'
ECUResetInvalidRPSID = b'\x7F\x11\x13'
InvalidRPDID = b'\x7F\x22\x13'

# Test different SIDs
if __name__ == '__main__':
    sendUDSPacketandPrint(TesterPresentSID)
    sendUDSPacketandPrint(DiagnosticSessionControlSID)
    sendUDSPacketandPrint(TesterPresentInvalidSID)

    sendUDSPacketandPrint(ECUResetInvalidSID)
    sendUDSPacketandPrint(ECUResetSID)

    # Wait a few seconds after reset then use Tester Present service
    time.sleep(5)
    sendUDSPacketandPrint(TesterPresentSID)

    # Print formatted time
    fprintTimefromHex(int_from_bytes(sendUDSPacketandPrint(ReadDataByIndentifierSID+TimeFromStartupDID)))
    time.sleep(1)
    fprintTimefromHex(int_from_bytes(sendUDSPacketandPrint(ReadDataByIndentifierSID+TimeFromStartupDID)))
    sendUDSPacketandPrint(ReadDataByIndentifierSID+InvalidDID)
