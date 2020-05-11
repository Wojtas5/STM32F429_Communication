from Testing import *
from UDSClient import *
import time

if __name__ == '__main__':
    suite1 = Suite('UDSTest')
    suite1.addTest('Services')
    suite1.EQ(TesterPresentRPSID, sendUDSPacketandPrint(TesterPresentSID))
    suite1.NE(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIndentifierSID + TimeFromStartupDID))
    suite1.EQ(ECUResetRPSID, sendUDSPacketandPrint(ECUResetSID))
    time.sleep(3)
    suite1.EQ(StopWatchStartRP, sendUDSPacketandPrint(RoutineControlSID + StopWatchStart + StopWatchRoutine))
    suite1.EQ(RoutineControlSeqErrorRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchStart + StopWatchRoutine))
    time.sleep(0.5)
    suite1.NE(RoutineControlSeqErrorRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine))

    printStopWatchTime(sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine))

    suite1.EQ(StopWatchStopRP, sendUDSPacketandPrint(RoutineControlSID + StopWatchStop + StopWatchRoutine))
    suite1.NE(RoutineControlSeqErrorRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine))

    printStopWatchTime(sendUDSPacketandPrint(RoutineControlSID + StopWatchSend + StopWatchRoutine))

    suite1.addTest('Errors')
    suite1.NE(DiagnosticSessionControlRPSID, sendUDSPacketandPrint(DiagnosticSessionControlSID))
    suite1.EQ(TesterPresentInvalidRPSID, sendUDSPacketandPrint(TesterPresentInvalidSID))
    suite1.EQ(ECUResetInvalidRPSID, sendUDSPacketandPrint(ECUResetInvalidSID))
    suite1.EQ(InvalidRPDID, sendUDSPacketandPrint(ReadDataByIndentifierSID + InvalidDID))
    suite1.EQ(RoutineControlSeqErrorRPSID, sendUDSPacketandPrint(RoutineControlSID + StopWatchStop + StopWatchRoutine))
    suite1.EQ(InvalidRoutineRPSID, sendUDSPacketandPrint(RoutineControlSID + InvalidRoutineSubfunc + StopWatchRoutine))
    suite1.EQ(RoutineControlSubfuncNotSupported, sendUDSPacketandPrint(RoutineControlSID + StopWatchRoutine))

    TEST(suite1)
