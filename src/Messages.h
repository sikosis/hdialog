#ifndef HDIALOG_MESSAGES_H
#define HDIALOG_MESSAGES_H

#include <SupportDefs.h>

constexpr uint32 kMessagePrimary = 'hdp1';
constexpr uint32 kMessageSecondary = 'hdp2';
constexpr uint32 kMessageTimeout = 'hdtm';
constexpr uint32 kMessageResult = 'hdrs';
constexpr uint32 kMessageSetTitle = 'hdst';
constexpr uint32 kMessageSetMessage = 'hdsm';
constexpr uint32 kMessageSetProgress = 'hdsp';
constexpr uint32 kMessageGetInput = 'hdgi';
constexpr uint32 kMessageChoosePath = 'hdcp';
constexpr uint32 kMessagePathSelected = 'hdps';
constexpr int32 kResultPrimary = 0;
constexpr int32 kResultSecondary = 2;
constexpr int32 kResultTimeout = 4;

#endif
