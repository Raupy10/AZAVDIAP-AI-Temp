#pragma once
#include <string>
#include <vector>

namespace Audio {

inline const std::string kNone = "-";

bool Init();
void Shutdown();
void RefreshDevices();

const std::vector<std::string>& InputNames();
const std::vector<std::string>& OutputNames();
const std::vector<std::string>& VirtualNames();

bool Start(const std::string& in, const std::string& out, const std::string& virt);
const std::string& Status();
bool HasVirtual();
const std::string& VirtualMic();

void SetMonitorMode(int mode);
void SetInputTest(bool on);
bool InputTest();
float InputLevel();
void PlayTestTone();

void Play(const std::string& path);
bool IsPlaying(const std::string& path);
void Stop(const std::string& path);
void StopAll();

}
