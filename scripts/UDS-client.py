from scapy.all import *
import time

def sendUDSPacketandPrint(msg):
    # Create a message
    p = Ether(dst='02:00:00:00:00:00', type=2048) / IP(ihl=5, src=srcaddress, dst=destaddress) / msg

    # Send the message
    sendp(p, iface=eth, count=1)

    # Wait for response
    r = sniff(filter='host 192.168.0.10',iface=eth, count=1)

    # Format the response to be visible as hex value
    packet = r[0]
    raw = packet.lastlayer()
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

# Service SIDs
TesterPresentSID = b'\x3E'
ECUResetSID = b'\x11'
ReadDataByIndentifierSID = b'\x22'
TimeFromStartupDID = b'\x01\x05'

# Other SIDs
DiagnosticSessionControlSID = b'\x10'
TesterPresentInvalidSID = b'\x3E\xFF\xFF'
ECUResetInvalidSID = b'\x11\3E'
InvalidDID = b'\x01\x06'

# Name of network interface card
eth = 'Realtek PCIe GBE Family Controller'

# Test different SIDs
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
