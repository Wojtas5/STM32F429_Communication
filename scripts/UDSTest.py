from Testing import *
from UDSClient import *
import time

if __name__ == '__main__':
    suite1 = Suite('UDSTest')
    suite1.addTest('Services')
    suite1.EQ(TesterPresentRPSID, sendUDSPacketandPrint(TesterPresentSID))
    suite1.NE(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIndentifierSID+TimeFromStartupDID))
    suite1.EQ(ECUResetRPSID , sendUDSPacketandPrint(ECUResetSID))
    time.sleep(3)

    suite1.addTest('Errors')
    suite1.NE(DiagnosticSessionControlRPSID, sendUDSPacketandPrint(DiagnosticSessionControlSID))
    suite1.EQ(TesterPresentInvalidRPSID, sendUDSPacketandPrint(TesterPresentInvalidSID))
    suite1.EQ(ECUResetInvalidRPSID, sendUDSPacketandPrint(ECUResetInvalidSID))
    suite1.EQ(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIndentifierSID + InvalidDID))

    TEST(suite1)

