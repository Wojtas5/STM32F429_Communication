from Testing import *
from UDSClient import *
import time

if __name__ == '__main__':
    TEST('UDSTest', 'Positive')
    EXPECT_EQ(sendUDSPacketandPrint(TesterPresentSID), TesterPresentRPSID)
    EXPECT_NE(sendUDSPacketandPrint(ReadDataByIndentifierSID+TimeFromStartupDID), InvalidRPDID)
    EXPECT_EQ(sendUDSPacketandPrint(ECUResetSID), ECUResetRPSID)
    time.sleep(3)

    TEST('UDSTest', 'Negative')
    EXPECT_NE(sendUDSPacketandPrint(DiagnosticSessionControlSID), DiagnosticSessionControlRPSID)
    EXPECT_EQ(sendUDSPacketandPrint(TesterPresentInvalidSID), TesterPresentInvalidRPSID)
    EXPECT_EQ(sendUDSPacketandPrint(ECUResetInvalidSID), ECUResetInvalidRPSID)
    EXPECT_EQ(sendUDSPacketandPrint(ReadDataByIndentifierSID+InvalidDID), InvalidRPDID)

    SUMMARY()


