#pragma once

void showPlayer();
void hidePlayer();
void launchModbusVideoPlayer();
bool isModbusVideoPlayerAlreadyRunning();
DWORD WINAPI startDaemonThread(LPVOID param = NULL);
void terminateDaemonThread();