from Testing import *
from UDSClient import *
import time

if __name__ == '__main__':
    suite1 = Suite('UDSTest')
    suite1.addTest('Basic Services')
    suite1.EQ(TesterPresentRPSID, sendUDSPacketandPrint(TesterPresentSID))
    suite1.NE(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIdentifierSID + TimeFromStartupDID))
    suite1.EQ(ECUResetRPSID, sendUDSPacketandPrint(ECUResetSID))
    time.sleep(3)

    suite1.addTest('Stopwatch routine')
    for i in range(17):
        suite1.EQ(StopWatchStartRP + StopWatchID[i], sendUDSPacketandPrint(RCStopwatchStart))

    suite1.EQ(RoutineControlServiceNotSupportedRPSID, sendUDSPacketandPrint(RCStopwatchStart))
    time.sleep(0.5)
    suite1.NE(RoutineControlIncMsgLenOrInvFormatRPSID, sendUDSPacketandPrint(RCStopwatchSend(1)))
    printStopWatchTime(sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine + StopWatchID[1]))
    suite1.EQ(StopWatchStopRP + StopWatchID[5], sendUDSPacketandPrint(RCStopwatchStop(5)))
    suite1.NE(RoutineControlIncMsgLenOrInvFormatRPSID, sendUDSPacketandPrint(RCStopwatchSend(5)))
    suite1.EQ(StopWatchStopRP + StopWatchID[1], sendUDSPacketandPrint(RCStopwatchStop(1)))
    suite1.EQ(RoutineControlIncMsgLenOrInvFormatRPSID, sendUDSPacketandPrint(RCStopwatchStop(1)))
    suite1.EQ(StopWatchStartRP + StopWatchID[5], sendUDSPacketandPrint(RCStopwatchStart))
    suite1.EQ(StopWatchStartRP + StopWatchID[1], sendUDSPacketandPrint(RCStopwatchStart))

    suite1.addTest('Errors')
    suite1.NE(DiagnosticSessionControlRPSID, sendUDSPacketandPrint(DiagnosticSessionControlSID))
    suite1.EQ(TesterPresentInvalidRPSID, sendUDSPacketandPrint(TesterPresentInvalidSID))
    suite1.EQ(ECUResetInvalidRPSID, sendUDSPacketandPrint(ECUResetInvalidSID))
    suite1.EQ(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIdentifierSID + InvalidDID))
    suite1.EQ(RoutineControlIncMsgLenOrInvFormatRPSID, sendUDSPacketandPrint(RoutineControlSID + InvalidRoutineSubfunc + StopWatchRoutine))
    suite1.EQ(RoutineControlSubfuncNotSupportedRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchRoutine))
    suite1.EQ(RoutineControlIncMsgLenOrInvFormatRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine + InvalidStopWatchID))

    TEST(suite1)
