#pragma once

#include <Windows.h>
#include "utils.h"
#include "bridge-layout.h"

std::wstring GetBridgeFilePath();

unsigned int readBridgeFileValue(std::wstring filePath);

void ScanForWatermark(unsigned int lowIntVal);

void SetupDataBridge();