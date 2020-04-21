from scapy.all import *

def sendUDSPacketandPrint(msg):
    # Create a message
    p = Ether(dst='02:00:00:00:00:00', type=2048) / IP(ihl=5, src=srcaddress, dst=destaddress) / msg

    # Send the message
    sendp(p, iface=eth, count=1)

    # Wait for response
    r = sniff(iface=eth, count=1)

    # Format the response to be visible as hex value
    packet = r[0]
    raw = packet.lastlayer()
    hexdump(raw)


# Addresses of microcontroller and PC
destaddress = '192.168.0.10'
srcaddress  = '192.168.0.11'

# Tester Present SID
TesterPresentSID = b'\x3E'

# Other SIDs
DiagnosticSessionControlSID = b'\x10'
TesterPresentInvalidSID = b'\x3E\xFF\xFF'

# Name of network interface card
eth = 'Realtek PCIe GBE Family Controller'

# Test three different SIDs
sendUDSPacketandPrint(TesterPresentSID)
sendUDSPacketandPrint(DiagnosticSessionControlSID)
sendUDSPacketandPrint(TesterPresentInvalidSID)




